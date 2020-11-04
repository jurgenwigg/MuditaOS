// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "HSP.hpp"
#include "Error.hpp"
#include "SCO.hpp"
#include <Audio/AudioCommon.hpp>

namespace Bt
{
    static constexpr int serviceBufferLength = 150;
    static constexpr int commandBufferLength = 150;

    class HSP::HSPImpl
    {
      public:
        static void packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize);
        auto init() -> Error::Code;
        void start();
        void stop();
        void setDeviceAddress(bd_addr_t addr);
        void setOwnerService(const sys::Service *service);
        auto getStreamData() -> std::shared_ptr<BluetoothStreamData>;

      private:
        static void sendAudioEvent(audio::EventType event);
        static void processHCIEvent(uint8_t *event);
        static void processHSPEvent(uint8_t *event);
        static std::array<uint8_t, serviceBufferLength> serviceBuffer;
        static constexpr uint8_t rfcommChannelNr = 1;
        static std::string agServiceName;
        static uint16_t scoHandle;
        static std::unique_ptr<SCO> sco;
        static std::array<char, commandBufferLength> ATcommandBuffer;
        static bd_addr_t deviceAddr;
        static const sys::Service *ownerService;
    };
} // namespace Bt