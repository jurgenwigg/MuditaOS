#pragma once

#include "Message.hpp"

namespace sys
{

    class Timer;

    class TimerMessage : public SystemMessage
    {
        Timer *timer = nullptr;

      public:
        TimerMessage(Timer *timer) : SystemMessage(SystemMessageType::Timer, ServicePowerMode::Active), timer(timer)
        {
            type = Type::System;
        }

        [[nodiscard]] auto getTimer() const -> Timer *
        {
            return timer;
        }
    };
} // namespace sys