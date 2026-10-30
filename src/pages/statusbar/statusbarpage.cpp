// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <memory>

#include "base/i18n.h"
#include "pages/statusbar/statusbarsettings.h"
#include "widgets/card.h"
#include "widgets/sections.h"
#include "widgets/sliderrow.h"
#include "window/page.h"
#include "window/pagelist.h"
#include "window/settingswindow.h"

using Lingmo::i18n::tr;

namespace settings {

namespace {

class StatusBarPage : public Page
{
public:
    explicit StatusBarPage(SettingsWindow& window)
        : Page(window, tr("Top Bar"))
    {
        Section* appearance = content()->addChild<Section>(tr("Appearance"));
        Card* card = appearance->addChild<Card>(Card::Style::List);

        SliderRow* blur = card->addChild<SliderRow>(tr("Blur"));
        blur->setName("statusbar:blur");
        blur->setCaptions(tr("Off"), tr("Strong"));
        // Logical pixels of Gaussian sigma, the unit the compositor takes.
        blur->setRange(0, StatusBarSettings::kMaxBlurSigma);
        blur->setStepSize(1);
        blur->setValue(m_settings.blurSigma());
        // The bar reports no changes back, so the row learns the new value
        // here; otherwise it returns to the old one once the move settles.
        blur->onMoved = [this, blur](double value) {
            m_settings.setBlurSigma(value);
            blur->setValue(m_settings.blurSigma());
        };

        SliderRow* transparency = card->addChild<SliderRow>(tr("Transparency"));
        transparency->setName("statusbar:transparency");
        transparency->setCaptions(tr("More solid"), tr("More clear"));
        transparency->setRange(0, 100);
        transparency->setStepSize(1);
        transparency->setValue((1.0 - m_settings.opacity()) * 100.0);
        transparency->onMoved = [this, transparency](double value) {
            m_settings.setOpacity(1.0 - value / 100.0);
            transparency->setValue((1.0 - m_settings.opacity()) * 100.0);
        };
    }

private:
    StatusBarSettings m_settings;
};

} // namespace

static const PageRegistration registration("statusbar", [](SettingsWindow& window) -> std::unique_ptr<Page> {
    return std::make_unique<StatusBarPage>(window);
});

} // namespace settings
