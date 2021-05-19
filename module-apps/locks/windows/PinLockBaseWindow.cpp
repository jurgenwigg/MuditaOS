// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "locks/data/LockStyle.hpp"
#include "locks/widgets/Lock.hpp"
#include <i18n/i18n.hpp>
#include "PinLockBaseWindow.hpp"
#include <Image.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void PinLockBaseWindow::buildImages(const std::string &lockImg, const std::string &infoImg)
    {
        lockImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, lockImg);
        infoImage = new gui::Image(this, lock_style::image::x, lock_style::image::y, 0, 0, infoImg);
    }

    auto PinLockBaseWindow::getToken(Token token) const -> std::string
    {
        if (token == Token::PinType) {
            return "$PINTYPE";
        }
        else if (token == Token::Sim) {
            return "$SIM";
        }
        else if (token == Token::Attempts) {
            return "$ATTEMPTS";
        }
        else if (token == Token::Mins) {
            return "$MINUTES";
        }
        else if (token == Token::CmeCode) {
            return "$CMECODE";
        }
        return std::string{};
    }

    top_bar::Configuration PinLockBaseWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        appConfiguration.enable(top_bar::Indicator::Battery);
        appConfiguration.enable(top_bar::Indicator::Signal);
        appConfiguration.enable(top_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void PinLockBaseWindow::restore() noexcept
    {
        LockWindow::restore();
        lockImage->setVisible(false);
        infoImage->setVisible(false);
    }

    void PinLockBaseWindow::setImagesVisible(bool lockImg, bool infoImg)
    {
        lockImage->setVisible(lockImg);
        infoImage->setVisible(infoImg);
    }

    void PinLockBaseWindow::setTitleBar(bool isVisible, bool isIceActive)
    {
        iceBox->setVisible(isIceActive);
        LockWindow::setTitleBar(isVisible);
    }

    void PinLockBaseWindow::buildBottomBar()
    {
        LockWindow::buildBottomBar();
        setBottomBarWidgetsActive(false, false, false);
    }

    void PinLockBaseWindow::buildTitleBar()
    {
        using namespace style::window::pin_lock;

        iceBox = new gui::HBox(this, ice::x, ice::y, ice::w, ice::h);
        iceBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceBox->setEdges(RectangleEdge::None);
        iceBox->setVisible(false);

        auto arrow        = new gui::Image("left_label_arrow");
        arrow->activeItem = false;
        arrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        arrow->setMargins(Margins(0, 0, ice::margin, 0));
        iceBox->addWidget(arrow);

        auto iceText        = new gui::Text(nullptr, 0, 0, ice::text::w, ice::h);
        iceText->activeItem = false;
        iceText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        iceText->setFont(style::window::font::verysmall);
        iceText->setText(utils::translate("app_desktop_emergency"));
        iceBox->addWidget(iceText);
    }

} // namespace gui