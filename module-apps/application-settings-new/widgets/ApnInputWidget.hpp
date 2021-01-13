// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/data/SettingsInternals.hpp"
#include "application-settings-new//widgets/ApnListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    class ApnInputWidget : public ApnListItem
    {
        settingsInternals::ListItemName listItemName;

      public:
        ApnInputWidget(settingsInternals::ListItemName listItemName,
                       std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                       std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                       std::function<void()> selectSpecialCharacter                 = nullptr,
                       std::function<void()> contentChanged                         = nullptr,
                       unsigned int lines                                           = 1);

      private:
        VBox *vBox                             = nullptr;
        Label *titleLabel                      = nullptr;
        TextFixedSize *inputText               = nullptr;
        std::function<void()> checkTextContent = nullptr;

        void applyItemNameSpecificSettings();
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        void nameHandler();
        void apnHandler();
        void usernameHandler();
        void passwordNumberHandler();
        void authtypeHandler();
        void apntypeHandler();
        void protocolHandler();
    };

} /* namespace gui */