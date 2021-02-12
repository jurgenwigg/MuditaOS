// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <json/json11.hpp>
#include <utility>

namespace gui
{

    class OnBoardingSwitchData : public gui::SwitchData
    {
      public:
        OnBoardingSwitchData() : SwitchData("onBoardingSwitchData")
        {}
    };
} // namespace gui