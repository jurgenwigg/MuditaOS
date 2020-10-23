// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AddDeviceWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"

#include "OptionSetting.hpp"

#include <i18/i18.hpp>
#include <utility>

namespace gui
{

    AddDeviceWindow::AddDeviceWindow(app::Application *app) : OptionWindow(app, gui::window::name::add_device)
    {}

    void AddDeviceWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            auto newData = static_cast<DeviceData *>(data);
            devices      = newData->getDevices();
        }
        rebuildOptionList();
    }

    auto AddDeviceWindow::devicesOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &device : devices) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                device.name,
                [=](gui::Item &item) {
                    LOG_DEBUG("Device: %s", device.name.c_str());
                    sys::Bus::SendUnicast(std::make_shared<BluetoothAddrMessage>(bd_addr_to_str(device.address)),
                                          "ServiceBluetooth",
                                          application,
                                          5000);
                    return true;
                },
                nullptr,
                nullptr,
                RightIcon::Bt));
        }

        sys::Bus::SendUnicast(
            std::make_shared<BluetoothMessage>(BluetoothMessage::Request::StopScan), "ServiceBluetooth", application);

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::add));

        return optionsList;
    }

    void AddDeviceWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(devicesOptionsList());
    }

} // namespace gui