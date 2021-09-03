// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::powerNapStyle
{
    inline constexpr auto descriptionFont = style::bell_sidelist_item::description_font;
    inline constexpr auto napPeriodFont   = style::window::font::supersizemelight;
    namespace listItem
    {
        inline constexpr auto timeUnitSingular = "common_minute_lower";
        inline constexpr auto timeUnitPlural   = "common_minutes_lower";
    } // namespace listItem
} // namespace gui::powerNapStyle