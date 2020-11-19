﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Message.hpp"

#include <module-bsp/bsp/torch/torch.hpp>

namespace sevm
{
    class BatteryLevelMessage : public Message
    {
      public:
        BatteryLevelMessage() : Message(MessageType::EVMBatteryLevel)
        {
            type = Type::Data;
        }
        uint8_t levelPercents = 0;
        bool fullyCharged     = false;
    };

    class BatteryPlugMessage : public Message
    {
      public:
        BatteryPlugMessage() : Message(MessageType::EVMChargerPlugged)
        {
            type = Type::Data;
        }
        bool plugged = false;
    };
} // namespace sevm