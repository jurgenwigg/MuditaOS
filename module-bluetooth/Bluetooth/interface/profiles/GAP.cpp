#include <Bluetooth/Device.hpp>
#include <log/log.hpp>
#include "Service/Bus.hpp"
#include <vector>
#include <Bluetooth/Error.hpp>
#include <module-services/service-bluetooth/messages/BluetoothMessage.hpp>
#include "BluetoothWorker.hpp"
#include "Device.hpp"

extern "C"
{
#include "btstack.h"
#include "hci.h"
};

btstack_packet_callback_registration_t cb_handler;

std::vector<Devicei> devices;

static auto start_scan() -> int;
static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

static auto getDeviceIndexForAddress(std::vector<Devicei> &devs, bd_addr_t addr) -> int
{
    int j;
    for (j = 0; j < devs.size(); j++) {
        if (bd_addr_cmp(addr, devs[j].address) == 0) {
            return j;
        }
    }
    return -1;
}

enum STATE
{
    INIT,
    ACTIVE,
    DONE
};
enum STATE state = INIT;

namespace Bt::GAP
{
    static sys::Service *ownerService = nullptr;

    void setOwnerService(sys::Service *service)
    {
        ownerService = service;
    }

    auto register_scan() -> Error
    {
        LOG_INFO("GAP register scan!");
        /// -> this have to be called prior to power on!
        hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
        cb_handler.callback = &packet_handler;
        hci_add_event_handler(&cb_handler);
        return Error();
    }

    auto scan() -> Error
    {
        LOG_INFO("Start scan if active: %d: %d", hci_get_state(), state);
        if (hci_get_state() == HCI_STATE_WORKING) {
            if (int ret = start_scan(); ret != 0) {
                LOG_ERROR("Start scan error!: 0x%X", ret);
                return Error(Error::LibraryError, ret);
            }
        }
        else {
            return Error(Error::NotReady);
        }
        return Error();
    }

    void stop_scan()
    {
        gap_inquiry_force_stop();
        LOG_INFO("Scan stopped!");
    }

    auto set_visibility(bool visibility) -> Error
    {
        gap_discoverable_control(static_cast<uint8_t>(visibility));
        LOG_INFO("Visibility: %s", visibility ? "true" : "false");
        return Error();
    }
} // namespace Bt::GAP

#define INQUIRY_INTERVAL 5
static auto start_scan() -> int
{
    LOG_INFO("Starting inquiry scan..");
    return gap_inquiry_start(INQUIRY_INTERVAL);
}

static auto has_more_remote_name_requests() -> int
{
    int i;
    for (i = 0; i < devices.size(); i++) {
        if (devices[i].state == REMOTE_NAME_REQUEST) {
            return 1;
        }
    }
    return 0;
}

static void do_next_remote_name_request()
{
    int i;
    for (i = 0; i < devices.size(); i++) {
        // remote name request
        if (devices[i].state == REMOTE_NAME_REQUEST) {
            devices[i].state = REMOTE_NAME_INQUIRED;
            LOG_INFO("Get remote name of %s...", bd_addr_to_str(devices[i].address));
            gap_remote_name_request(
                devices[i].address, devices[i].pageScanRepetitionMode, devices[i].clockOffset | 0x8000);
            return;
        }
    }
}

static void continue_remote_names()
{
    if (has_more_remote_name_requests() != 0) {
        do_next_remote_name_request();
        return;
    }
    start_scan();
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(channel);
    UNUSED(size);

    bd_addr_t addr;
    int i;
    int index;

    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }

    uint8_t event = hci_event_packet_get_type(packet);

    switch (state) {
        /* @text In INIT, an inquiry  scan is started, and the application transits to
         * ACTIVE state.
         */
    case INIT:
        switch (event) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                state = ACTIVE;
            }
            break;
        default:
            break;
        }
        break;

        /* @text In ACTIVE, the following events are processed:
         *  - GAP Inquiry result event: BTstack provides a unified inquiry result that contain
         *    Class of Device (CoD), page scan mode, clock offset. RSSI and name (from EIR) are optional.
         *  - Inquiry complete event: the remote name is requested for devices without a fetched
         *    name. The state of a remote name can be one of the following:
         *    REMOTE_NAME_REQUEST, REMOTE_NAME_INQUIRED, or REMOTE_NAME_FETCHED.
         *  - Remote name request complete event: the remote name is stored in the table and the
         *    state is updated to REMOTE_NAME_FETCHED. The query of remote names is continued.
         */
    case ACTIVE:
        switch (event) {

        case GAP_EVENT_INQUIRY_RESULT: {
            gap_event_inquiry_result_get_bd_addr(packet, addr);
            index = getDeviceIndexForAddress(devices, addr);
            if (index >= 0) {
                break; // already in our list
            }

            Devicei dev;
            dev.address_set(&addr);
            dev.pageScanRepetitionMode = gap_event_inquiry_result_get_page_scan_repetition_mode(packet);
            dev.clockOffset            = gap_event_inquiry_result_get_clock_offset(packet);
            // print info
            LOG_INFO("Device found: %s ", bd_addr_to_str(addr));
            LOG_INFO("with COD: 0x%06x, ", (unsigned int)gap_event_inquiry_result_get_class_of_device(packet));
            LOG_INFO("pageScan %d, ", dev.pageScanRepetitionMode);
            LOG_INFO("clock offset 0x%04x", dev.clockOffset);
            if (gap_event_inquiry_result_get_rssi_available(packet) != 0u) {
                LOG_INFO(", rssi %d dBm", (int8_t)gap_event_inquiry_result_get_rssi(packet));
            }
            if (gap_event_inquiry_result_get_name_available(packet) != 0u) {
                char name_buffer[240];
                int name_len = gap_event_inquiry_result_get_name_len(packet);
                memcpy(name_buffer, gap_event_inquiry_result_get_name(packet), name_len);
                name_buffer[name_len] = 0;
                LOG_INFO(", name '%s'", name_buffer);
                dev.name  = std::string{name_buffer};
                dev.state = REMOTE_NAME_FETCHED;
            }
            else {
                dev.state = REMOTE_NAME_REQUEST;
            }
            devices.push_back(dev);
            auto msg = std::make_shared<BluetoothScanMessage>(devices);
            auto ret = sys::Bus::SendUnicast(msg, "ApplicationSettings", Bt::GAP::ownerService, 5000);

        } break;

        case GAP_EVENT_INQUIRY_COMPLETE:
            for (i = 0; i < devices.size(); i++) {
                // retry remote name request
                if (devices[i].state == REMOTE_NAME_INQUIRED)
                    devices[i].state = REMOTE_NAME_REQUEST;
            }
            continue_remote_names();
            break;

        case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE: {
            reverse_bd_addr(&packet[3], addr);
            index = getDeviceIndexForAddress(devices, addr);
            if (index >= 0) {
                if (packet[2] == 0) {
                    LOG_INFO("Name: '%s'", &packet[9]);
                    devices[index].state = REMOTE_NAME_FETCHED;
                    devices[index].name  = reinterpret_cast<char *>(&packet[9]);
                }
                else {
                    LOG_INFO("Failed to get name: page timeout");
                }
            }
            if (index + 1 == devices.size()) {
                LOG_INFO("Scanned all");
                state = DONE;
                gap_inquiry_stop();
                break;
            }
            continue_remote_names();
        } break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}