// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pwm/DriverPWM.hpp"

#include "../fsl_drivers/fsl_common.h"
#include "../fsl_drivers/fsl_pwm.h"

#include "mutex.hpp"

namespace drivers
{
    class RT1051DriverPWM : public DriverPWM
    {
      public:
        RT1051DriverPWM(PWMInstances inst, PWMModules mod, const DriverPWMParams &params);

        ~RT1051DriverPWM() final;

        void SetDutyCycle(std::uint8_t duty_cycle_percent) final;

        void Start() final;

        void Stop() final;

      private:
        void SetupPWMChannel(PWMChannel channel, std::uint32_t frequency);

        void ForceLowOutput();

        void RestorePwmOutput();

        PWM_Type *base = nullptr;

        pwm_submodule_t pwmModule = kPWM_Module_0;

        pwm_signal_param_t pwmSignalConfig = {.pwmChannel       = kPWM_PwmB,
                                              .dutyCyclePercent = 0,
                                              .level            = kPWM_HighTrue,
                                              .deadtimeValue    = 0,
                                              .faultState       = kPWM_PwmFaultState0};

        std::uint8_t lastDutyCycle = 0;

        cpp_freertos::MutexStandard dutyCycleMutex;
    };

} // namespace drivers