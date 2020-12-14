// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <vector>
#include <memory>

namespace sevm::screen_light_control::functions
{
    using BrightnessFunction =
        std::vector<std::pair<bsp::light_sensor::IlluminanceLux, bsp::eink_frontlight::BrightnessPercentage>>;

    bsp::eink_frontlight::BrightnessPercentage brightnessRampOut();

    void calculateBrightness(bsp::light_sensor::IlluminanceLux measurement);

    void setRampStep(float step);

    void setHysteresis(float hyst);

    void setFunctionFromPoints(const BrightnessFunction &points);

} // namespace sevm::screen_light_control::functions