﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnSettingsWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/data/ApnListData.hpp"
#include "application-settings-new/models/ApnSettingsModel.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"
#include "application-settings-new/data/SettingsItemData.hpp"
#include "OptionSetting.hpp"

#include <InputEvent.hpp>

namespace gui
{

    ApnSettingsWindow::ApnSettingsWindow(app::Application *app) : OptionWindow(app, gui::window::name::apn_settings)
    {
        buildInterface();
    }

    void ApnSettingsWindow::buildInterface()
    {
        setTitle(utils::localize.get("app_settings_network_apn_settings"));

        leftArrowImage = new gui::Image(this,
                                        style::settings::window::leftArrowImage::x,
                                        style::settings::window::leftArrowImage::y,
                                        style::settings::window::leftArrowImage::w,
                                        style::settings::window::leftArrowImage::h,
                                        "arrow_left");
        crossImage     = new gui::Image(this,
                                    style::settings::window::crossImage::x,
                                    style::settings::window::crossImage::y,
                                    style::settings::window::crossImage::w,
                                    style::settings::window::crossImage::h,
                                    "cross");
        emptyListIcon  = new Icon(this,
                                 0,
                                 style::header::height,
                                 style::window_width,
                                 style::window_height - style::header::height - style::footer::height,
                                 "phonebook_empty_grey_circle_W_G",
                                 utils::localize.get("app_settings_apn_settings_no_apns"));

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));

        auto apnSettingsModel = new ApnSettingsModel(application);
        apnSettingsModel->requestAPNList();
    }
    auto ApnSettingsWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        const auto newData = dynamic_cast<ApnListData *>(data);
        if (newData == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        apns = newData->getAPNs();
        if (apns.empty()) {
            emptyListIcon->setVisible(true);
            return false;
        }

        return true;
    }

    void ApnSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        clearOptions();
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        emptyListIcon->setVisible(false);

        if (apns.empty()) {
            return;
        }

        addOptions(optionsList(apns));
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
    }

    auto ApnSettingsWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if (!inputEvent.isShortPress()) {
            return false;
        }
        if (inputEvent.is(gui::KeyCode::KEY_LEFT)) {
            auto apnRecord                        = std::make_shared<packet_data::APN::Config>();
            std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(apnRecord);
            application->switchWindow(gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }
        if (inputEvent.is(gui::KeyCode::KEY_LF)) {
            auto apnRecord                        = std::make_shared<packet_data::APN::Config>();
            std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(apnRecord);
            application->switchWindow(gui::window::name::apn_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return true;
        }

        return false;
    }

    auto ApnSettingsWindow::optionsList(std::vector<std::shared_ptr<packet_data::APN::Config>> apnsList)
        -> std::list<Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &apn : apnsList) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                (apn->apnType == packet_data::APN::APNType::Default) ? "<b>" + apn->apn + "</b>" : apn->apn,
                [=](gui::Item &item) {
                    LOG_DEBUG("APN: %s", apn->apn.c_str());
                    std::unique_ptr<gui::SwitchData> apnData = std::make_unique<ApnItemData>(apn);
                    application->switchWindow(
                        gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(apnData));
                    return true;
                },
                nullptr,
                nullptr));
        }

        return optionsList;
    }

} // namespace gui