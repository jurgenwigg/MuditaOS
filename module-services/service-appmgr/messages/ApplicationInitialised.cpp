// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/ApplicationInitialised.hpp>

namespace app::manager
{
    ApplicationInitialised::ApplicationInitialised(const ApplicationName &senderName,
                                                   StartupStatus _status,
                                                   StartInBackground _startInBackground)
        : BaseMessage(MessageType::APMInit, senderName), status{_status}, startInBackground{_startInBackground}
    {}

    [[nodiscard]] auto ApplicationInitialised::getStatus() const noexcept -> StartupStatus
    {
        return status;
    }

    [[nodiscard]] auto ApplicationInitialised::isBackgroundApplication() const noexcept -> bool
    {
        return startInBackground.value;
    }
} // namespace app::manager