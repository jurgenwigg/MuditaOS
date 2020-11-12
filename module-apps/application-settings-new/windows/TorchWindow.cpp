// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TorchWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include "OptionSetting.hpp"

#include <i18/i18.hpp>

namespace gui
{

    TorchWindow::TorchWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::torch)
    {
        buildInterface();
    }

    void TorchWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::translateI18("app_settings_title_torch"));
        body->setSize(body->getWidth(), body->getHeight() - style::settings::window::torch::body_offset);
        bar = new Rect(this,
                       style::window::default_left_margin,
                       style::settings::window::torch::bar_y,
                       style::window::default_body_width,
                       style::settings::window::torch::bar_h);
        bar->setVisible(false);
        descriptionText = new Text(this,
                                   style::window::default_left_margin,
                                   style::settings::window::torch::description_y,
                                   style::window::default_body_width,
                                   style::settings::window::torch::description_h);
        descriptionText->setFont(style::window::font::medium);
        descriptionText->setText(utils::translateI18("app_settings_torch_description"));
        descriptionText->setVisible(false);
    }

    auto TorchWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_torch_sunset_red_light_option"),
            [=](gui::Item &item) {
                toggleSwitchState();
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            switchState ? RightItem::On : RightItem::Off));

        if (switchState) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_torch_nightshift_time_option"),
                [=](gui::Item &item) { return true; },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                nullptr,
                RightItem::ArrowWhite));
        }

        return optionsList;
    }

    void TorchWindow::toggleSwitchState()
    {
        switchState = !switchState;
        rebuildOptionList();
        bar->setVisible(switchState);
        descriptionText->setVisible(switchState);
    }

} // namespace gui