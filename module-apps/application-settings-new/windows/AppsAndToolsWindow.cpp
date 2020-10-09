#include "AppsAndToolsWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>
#include <OptionWindow.hpp>

namespace gui
{
    AppsAndToolsWindow::AppsAndToolsWindow(app::Application *app) : OptionWindow(app, gui::window::name::apps_and_tools)
    {
        addOptions(appsAndToolsOptionsList());
        setTitle(utils::localize.get("app_settings_apps_tools"));
    }

    std::list<Option> AppsAndToolsWindow::appsAndToolsOptionsList()
    {
        std::list<gui::Option> optionList;

        auto i18     = [](std::string text) { return utils::localize.get(text); };
        auto addMenu = [&](UTF8 name, std::string window = "") {
            optionList.emplace_back(gui::Option{name,
                                                [=](gui::Item &item) {
                                                    if (window == "") {
                                                        return false;
                                                    }
                                                    LOG_INFO("switching to %s page", window.c_str());
                                                    application->switchWindow(window, nullptr);
                                                    return true;
                                                },
                                                gui::Arrow::Enabled});
        };

        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);

        addMenu(i18("app_settings_apps_messages"), gui::window::name::messages);
        addMenu(i18("app_settings_apps_torch"), gui::window::name::torch);

        return optionList;
    }
} // namespace gui