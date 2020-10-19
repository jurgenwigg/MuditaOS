#include "MeditationTimerWindow.hpp"

#include "application-meditation/widgets/MeditationTimer.hpp"
#include "application-meditation/data/Style.hpp"
#include "application-meditation/data/MeditationTimerData.hpp"
#include "Names.hpp"

#include <cassert>
#include <i18/i18.hpp>

#include "RichTextParser.hpp"
#include "FontManager.hpp"

using namespace gui;

MeditationTimerWindow::MeditationTimerWindow(app::Application *app) : AppWindow{app, name::window::main_window}
{
    MeditationTimerWindow::buildInterface();
}

void MeditationTimerWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void MeditationTimerWindow::buildInterface()
{
    auto app = dynamic_cast<app::ApplicationMeditation *>(application);
    assert(app != nullptr); // Pre-condition check.

    AppWindow::buildInterface();
    setTitle(utils::localize.get("app_meditation_title_main"));

    timer = new MeditationTimer(style::meditation::timer::X,
                                style::meditation::timer::Y,
                                style::meditation::timer::Width,
                                style::meditation::timer::Height,
                                app,
                                this);

    meditationInfo = new Text(this,
                              style::meditation::timer::infoText::X,
                              style::meditation::timer::infoText::Y,
                              style::meditation::timer::infoText::Width,
                              style::meditation::timer::infoText::Height);
    meditationInfo->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
}

void MeditationTimerWindow::destroyInterface()
{
    erase();
    invalidate();
}

void MeditationTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto timerData = dynamic_cast<MeditationTimerData *>(data);
    assert(timerData);
    setVisiblePreparation();
    meditationTime = timerData->getMeditationTime();

    auto onPreparation = [&]() -> void {
        setVisibleRunning();
        auto onMeditationEnd = [&]() -> void {
            setVisibleMeditationEnd();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        };
        timer->registerTimeoutCallback(onMeditationEnd);
        timer->reset(meditationTime);
        timer->start();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    };

    timer->registerTimeoutCallback(onPreparation);
    timer->setTimerVisible(timerData->isCounterEnabled());
    timer->reset(timerData->getPreparationTime());
    timer->start();
}

auto MeditationTimerWindow::onInput(const InputEvent &inputEvent) -> bool
{
    if (inputEvent.isShortPress()) {
        if (finished) {
            application->switchWindow(app::window::name::meditation_main_window);
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_RF) && bottomBar->isActive(BottomBar::Side::RIGHT)) {
            timer->stop();
            finished = true;
            setVisibleMeditationEnd();
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
            if (timer->isStopped()) {
                timer->start();
                bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::pause));
                setWidgetVisible(false, true, true);
            }
            else {
                timer->stop();
                bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::start));
                setWidgetVisible(true, true, true);
            }
            return true;
        }
    }

    return AppWindow::onInput(inputEvent);
}

void MeditationTimerWindow::setWidgetVisible(bool tBar, bool bBar, bool counter)
{
    topBar->setActive(TopBar::Elements::TIME, tBar);
    title->setVisible(tBar);
    bottomBar->setActive(BottomBar::Side::CENTER, bBar);
    bottomBar->setActive(BottomBar::Side::LEFT, bBar);
    timer->setVisible(counter);
}
void MeditationTimerWindow::setVisibleRunning()
{
    setWidgetVisible(false, true, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::pause));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::stop));
    meditationInfo->setVisible(false);
}
void MeditationTimerWindow::setVisiblePaused()
{
    setWidgetVisible(true, true, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::resume));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::stop));
}
void MeditationTimerWindow::setVisiblePreparation()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::localize.get("app_meditation_put_down_phone_and_wait"), &format);
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);

    finished = false;
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
}

void MeditationTimerWindow::setVisibleMeditationEnd()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::localize.get("app_meditation_thank_you_for_session"), &format);
    finished        = true;
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);
    bottomBar->setVisible(false);
}

void MeditationTimerWindow::invalidate() noexcept
{
    timer          = nullptr;
    meditationInfo = nullptr;
}