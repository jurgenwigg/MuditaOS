#include "PinLockBaseWindow.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include <i18/i18.hpp>

namespace lock_style = style::window::pin_lock;
namespace gui
{

    void PinLockBaseWindow::build()
    {
        buildBottomBar();
        buildTopBar();
        buildTitleLabel();
    }
    void PinLockBaseWindow::buildBottomBar()
    {
        setBottomBarWidgetsActive(false, false, false);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_emergency"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }
    void PinLockBaseWindow::buildTopBar()
    {
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::LOCK, true);
    }
    void PinLockBaseWindow::buildTitleLabel()
    {
        titleLabel = new gui::Label(this, 0, lock_style::title_label_y, style::window_width, lock_style::title_label_h);
        titleLabel->setFilled(false);
        titleLabel->setVisible(false);
        titleLabel->setBorderColor(gui::ColorFullBlack);
        titleLabel->setFont(style::header::font::title);
        titleLabel->setText(utils::localize.get("app_desktop_pin_info1"));
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
    }
    void PinLockBaseWindow::buildInfoText(unsigned int h)
    {
        infoText = new Text(this, 0, lock_style::info_text_y, style::window_width, h);
        infoText->setFont(style::window::font::medium);
        infoText->setVisible(true);
        infoText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }
    void PinLockBaseWindow::buildPinLabels(gui::Label *labelsBox, unsigned int pinSize, unsigned int singleLabelWidth)
    {
        if (pinSize * singleLabelWidth > labelsBox->getWidth()) {
            singleLabelWidth = labelsBox->getWidth() / pinSize;
        }
        const uint32_t pinLabelSpacing = (labelsBox->getWidth() - pinSize * singleLabelWidth) / (pinSize - 1);

        uint32_t pinLabelX = 0;
        for (uint32_t i = 0; i < pinSize; i++) {
            gui::Label *label = new gui::Label(labelsBox, pinLabelX, 0, singleLabelWidth, lock_style::label_size);
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);
            label->setPenWidth(2);
            label->setFont(style::window::font::largelight);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
            label->setVisible(true);
            this->pinLabels.push_back(label);
            pinLabelX += singleLabelWidth + pinLabelSpacing;
        }
    }
    void PinLockBaseWindow::buildImages(const std::string &lockImg, const std::string &infoImg)
    {
        lockImage = new gui::Image(this, lock_style::image_x, lock_style::image_y, 0, 0, lockImg);
        infoImage = new gui::Image(this, lock_style::image_x, lock_style::image_y, 0, 0, infoImg);
        infoImage->setVisible(false);
    }
    void PinLockBaseWindow::setBottomBarWidgetsActive(bool left, bool center, bool right)
    {
        bottomBar->setActive(BottomBar::Side::LEFT, left);
        bottomBar->setActive(BottomBar::Side::CENTER, center);
        bottomBar->setActive(BottomBar::Side::RIGHT, right);
    }
    void PinLockBaseWindow::setImagesVisible(bool lockImg, bool infoImg)
    {
        lockImage->setVisible(lockImg);
        infoImage->setVisible(infoImg);
    }
    void PinLockBaseWindow::setBottomBarWidgetText(BottomBar::Side side, const UTF8 &txt)
    {
        bottomBar->setText(side, txt, false);
    }
    void PinLockBaseWindow::clearPinLabels()
    {
        for (auto label : pinLabels) {
            label->clear();
        }
    }
} // namespace gui