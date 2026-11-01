// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "base/i18n.h"
#include "base/process.h"
#include "base/timer.h"
#include "lingmo/appearance/appearance.h"
#include "pages/about/aboutinfo.h"
#include "ui/cell_layout.h"
#include "ui/clipboard.h"
#include "ui/image_view.h"
#include "ui/label.h"
#include "ui/painter.h"
#include "ui/theme.h"
#include "ui/view.h"
#include "widgets/card.h"
#include "widgets/listrow.h"
#include "widgets/metrics.h"
#include "widgets/picture.h"
#include "widgets/sections.h"
#include "widgets/standardbutton.h"
#include "window/page.h"
#include "window/pagelist.h"

using namespace Lingmo::ui;
using Lingmo::i18n::tr;

namespace settings {

namespace {

// The rule between the wordmark and the wallpaper, at the height of the text
// rather than of the picture beside it.
class VerticalRule : public View
{
protected:
    void paint(Painter& painter) override
    {
        painter.drawVerticalLine(geometry().width / 2, 0, geometry().height,
                                 Theme::current().disabledText.withAlpha(0.28), 1);
    }
};

// The current wallpaper, rounded and framed: the desktop's own picture is the
// most recognisable thing on this page. A wallpaper is usually a photo, so the
// picture is an ImageView (SVG files included), not the SVG-only Picture.
class WallpaperThumbnail : public View
{
public:
    explicit WallpaperThumbnail(std::string path)
    {
        m_picture = addChild<ImageView>(std::move(path));
        m_picture->setFillMode(ImageView::FillMode::PreserveAspectCrop);
        m_picture->setCornerRadius(Theme::current().bigRadius);
        m_frame = addChild<Frame>();
    }

protected:
    void layout() override
    {
        m_picture->setGeometry(localRect());
        m_frame->setGeometry(localRect());
    }

    void paint(Painter& painter) override
    {
        const Theme& theme = Theme::current();
        // The shadow lifts the picture off the card; the frame is drawn by a
        // child after the picture, so its hairline stays visible.
        painter.drawShadow(localRect(), theme.bigRadius, 18,
                           Color {0, 0, 0, uint8_t(theme.darkMode ? 90 : 38)}, {0, 4});
    }

private:
    class Frame : public View
    {
    protected:
        void paint(Painter& painter) override
        {
            const Theme& theme = Theme::current();
            painter.drawSmoothRectangle(localRect(), theme.bigRadius, Color {0, 0, 0, 0}, 1,
                                        theme.text.withAlpha(0.18));
        }
    };

    ImageView* m_picture = nullptr;
    Frame* m_frame = nullptr;
};

// One cell of the system grid: what it is above, the value below. The grid is
// two rows of four, so the label cannot sit beside the value as in a row.
class InfoCell : public View
{
public:
    InfoCell(std::string key, std::string value)
    {
        // The grid's columns are as wide as their cells ask to be; without
        // this a cell takes its preferred width, which is nothing.
        setLayoutHints({.fillWidth = true});
        m_key = addChild<Label>(std::move(key));
        m_key->setColor(Theme::current().disabledText);
        m_key->setFontSize(metrics::points(9));
        m_value = addChild<Label>(std::move(value));
        m_value->setWrap(true);
        m_value->setFontSize(metrics::points(10));
        m_value->setWeight(500);
    }

protected:
    void layout() override
    {
        const double width = geometry().width;
        const double keyHeight = m_key->heightForWidth(width);
        m_key->setGeometry({0, 0, width, keyHeight});
        m_value->setGeometry({0, keyHeight + metrics::xs, width, m_value->heightForWidth(width)});
    }

    // The grid measures a row at the width it has to fill, so this is what
    // decides the cell's height, not sizeHint(): before layout there is no
    // width to wrap the value at.
    double heightForWidth(double width) const override
    {
        return m_key->heightForWidth(width) + metrics::xs + m_value->heightForWidth(width);
    }

private:
    Label* m_key;
    Label* m_value;
};

class AboutPage : public Page
{
public:
    explicit AboutPage(SettingsWindow& window)
        : Page(window, tr("About"))
    {
        addBrand();
        addIdentity();
        addSystem();
        addActions();
    }

protected:
    void layout() override
    {
        // The wordmark is drawn dark for the light theme and white for the
        // dark one, so the file is swapped rather than the picture tinted.
        if (m_logo) {
            const std::string logo = logoForTheme();
            if (logo != m_logoPath) {
                m_logoPath = logo;
                m_logo->setPath(logo);
            }
        }
        Page::layout();
    }

private:
    static std::string logoForTheme()
    {
        return dataPath(Theme::current().darkMode ? "images/logo-dark.svg" : "images/logo-light.svg");
    }

    // The wordmark, the rule, and the wallpaper the session is showing, under
    // the product name and the machine's model.
    void addBrand()
    {
        Card* hero = content()->addChild<Card>();
        RowLayout* brand = hero->addChild<RowLayout>();
        brand->setSpacing(metrics::xl);
        brand->setLayoutHints({.horizontalAlignment = Alignment::Center});

        m_logoPath = logoForTheme();
        m_logo = brand->addChild<Picture>(m_logoPath);
        m_logo->setLayoutHints({.preferredWidth = 220, .preferredHeight = 84,
                                .verticalAlignment = Alignment::Center});

        VerticalRule* rule = brand->addChild<VerticalRule>();
        rule->setLayoutHints({.preferredWidth = 1, .preferredHeight = 56,
                              .verticalAlignment = Alignment::Center});

        WallpaperThumbnail* thumbnail = brand->addChild<WallpaperThumbnail>(m_appearance.wallpaper());
        thumbnail->setLayoutHints({.preferredWidth = 160, .preferredHeight = 96,
                                   .verticalAlignment = Alignment::Center});

        const std::string product = about::prettyProductName();
        Label* productLabel =
                hero->addChild<Label>(product.find("Lingmo") != std::string::npos ? product : tr("Lingmo OS"));
        productLabel->setTextAlignment(Label::TextAlignment::Center);
        productLabel->setFontSize(metrics::points(10));
        productLabel->setWeight(500);
        LayoutHints productHints;
        productHints.fillWidth = true;
        productHints.margins.top = metrics::s;
        productLabel->setLayoutHints(productHints);
    }

    void addIdentity()
    {
        Section* section = content()->addChild<Section>(tr("Device"));
        Card* card = section->addChild<Card>(Card::Style::List);
        card->addChild<InfoRow>(tr("PC Name"), about::hostName());
        card->addChild<InfoRow>(tr("OS Version"), about::prettyProductName());
        const std::string model = about::computerModel();
        if (!model.empty())
            card->addChild<InfoRow>(tr("Computer Model"), model);
    }

    // Two rows of four: what the system is made of.
    void addSystem()
    {
        Section* section = content()->addChild<Section>(tr("System"));
        // A padded card: a grid is not a list row and takes the card's padding.
        Card* card = section->addChild<Card>(Card::Style::Padded);
        GridLayout* grid = card->addChild<GridLayout>(4);
        grid->setLayoutHints({.fillWidth = true});
        grid->setColumnSpacing(metrics::l);
        grid->setRowSpacing(metrics::m);

        grid->addChild<InfoCell>(tr("Debian Version"), about::debianVersion());
        grid->addChild<InfoCell>(tr("Linux Kernel Version"), "Linux " + about::kernelVersion());
        grid->addChild<InfoCell>(tr("Lingmo UI Version"), about::uiVersion());
        grid->addChild<InfoCell>(tr("System Type"), about::architecture());

        grid->addChild<InfoCell>(tr("RAM"), about::memorySize());
        grid->addChild<InfoCell>(tr("Disk"), about::internalStorage());
        grid->addChild<InfoCell>(tr("CPU"), about::processor());
        // The eighth cell stays empty, so the second row keeps the first one's
        // four columns.
        grid->addChild<View>();
    }

    void addActions()
    {
        Section* section = content()->addChild<Section>(tr("Support"));
        Card* card = section->addChild<Card>(Card::Style::List);
        NavigationItem* update = card->addChild<NavigationItem>(tr("System Update"));
        update->onClicked = [] { base::Process::startDetached("lingmo-updator"); };

        m_copy = content()->addChild<StandardButton>(tr("Copy System Information"));
        m_copy->setName("about:copy-info");
        m_copy->onClicked = [this] {
            Clipboard::setText(systemInformation());
            m_copy->setText(tr("Copied"));
            m_copyReset.start();
        };
        m_copyReset.setSingleShot(true);
        m_copyReset.setInterval(std::chrono::milliseconds(1600));
        m_copyReset.onTimeout = [this] {
            if (m_copy)
                m_copy->setText(tr("Copy System Information"));
        };
    }

    std::string systemInformation() const
    {
        std::string text = about::prettyProductName() + "\n";
        const auto line = [&text](const std::string& key, const std::string& value) {
            if (!value.empty())
                text += key + ": " + value + "\n";
        };
        line(tr("PC Name"), about::hostName());
        line(tr("Computer Model"), about::computerModel());
        line(tr("Debian Version"), about::debianVersion());
        line(tr("Linux Kernel Version"), about::kernelVersion());
        line(tr("Lingmo UI Version"), about::uiVersion());
        line(tr("System Type"), about::architecture());
        line(tr("CPU"), about::processor());
        line(tr("RAM"), about::memorySize());
        line(tr("Disk"), about::internalStorage());
        return text;
    }

    Picture* m_logo = nullptr;
    std::string m_logoPath;
    StandardButton* m_copy = nullptr;
    base::Timer m_copyReset;
    Lingmo::appearance::Appearance m_appearance;
};

} // namespace

static const PageRegistration registration("about", [](SettingsWindow& window) -> std::unique_ptr<Page> {
    return std::make_unique<AboutPage>(window);
});

} // namespace settings
