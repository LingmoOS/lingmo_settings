// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <memory>

#include "base/dbus.h"
#include "base/i18n.h"
#include "ui/label.h"
#include "widgets/card.h"
#include "widgets/sections.h"
#include "widgets/standardbutton.h"
#include "window/page.h"
#include "window/pagelist.h"
#include "window/settingswindow.h"

using Lingmo::i18n::tr;

namespace settings {

namespace {

// The desktop's widgets live in the file manager's desktop window; this asks
// its D-Bus service to open the gallery there.
void openWidgetGallery()
{
    dbus::Proxy(dbus::Connection::session(), "org.lingmo.FileManager", "/FileManager",
                "org.lingmo.FileManager")
            .send("EditWidgets");
}

class DesktopWidgetsPage : public Page
{
public:
    explicit DesktopWidgetsPage(SettingsWindow& window)
        : Page(window, tr("Desktop Widgets"))
    {
        Card* card = content()->addChild<Card>();
        card->setLayoutHints({.fillWidth = true});
        Lingmo::ui::Label* description = card->addChild<Lingmo::ui::Label>(
                tr("Add clock, calendar, system and battery cards to your desktop, then drag them where you "
                   "like."));
        description->setWrap(true);
        description->setLayoutHints({.fillWidth = true});

        StandardButton* edit = content()->addChild<StandardButton>(tr("Edit Widgets…"));
        edit->setName("desktopwidgets:edit");
        edit->onClicked = [] { openWidgetGallery(); };

        content()->addChild<Footnote>(
                tr("A widget's place is saved for the display it is on. Right-click a card to remove it or "
                   "change its size."));
    }
};

} // namespace

static const PageRegistration registration("desktopwidgets", [](SettingsWindow& window) -> std::unique_ptr<Page> {
    return std::make_unique<DesktopWidgetsPage>(window);
});

} // namespace settings
