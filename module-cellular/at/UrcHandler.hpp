// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace at::urc
{
    class Clip;
    class Creg;
    class Cmti;
    class Cusd;
    class Ctze;
    class Qind;
    class PoweredDown;
    class UrcResponse;

    class UrcHandler
    {
      public:
        virtual void Handle(Clip &urc)        = 0;
        virtual void Handle(Creg &urc)        = 0;
        virtual void Handle(Cmti &urc)        = 0;
        virtual void Handle(Cusd &urc)        = 0;
        virtual void Handle(Ctze &urc)        = 0;
        virtual void Handle(Qind &urc)        = 0;
        virtual void Handle(PoweredDown &urc) = 0;
        virtual void Handle(UrcResponse &urc) = 0;
    };
} // namespace at::urc