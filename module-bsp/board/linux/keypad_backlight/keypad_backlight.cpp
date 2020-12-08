// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

namespace bsp::keypad_backlight
{
    namespace
    {
        bool diodesState = false;
    }

    std::int32_t init()
    {
        return 0;
    }

    void deinit()
    {}

    bool turnOnAll()
    {
        diodesState = true;
        return true;
    }

    bool configureModule()
    {
        return true;
    }

    bool shutdown()
    {
        diodesState = false;
        return true;
    }

    void wakeup()
    {}

    bool reset()
    {
        return true;
    }

    bool checkState()
    {
        return diodesState;
    }

} // namespace bsp::keypad_backlight