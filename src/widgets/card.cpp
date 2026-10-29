// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "widgets/card.h"

#include "ui/painter.h"
#include "ui/theme.h"
#include "widgets/metrics.h"

using namespace Lingmo::ui;

namespace settings {

// The sidebar's own background is drawn at 0.7. A card sits a shade clearer
// than that: close enough to the wallpaper to read as the same glass, opaque
// enough to hold the text on it.
static constexpr double kCardOpacity = 0.6;

Card::Card(Style style)
{
    if (style == Style::Padded) {
        setPadding({metrics::cardHorizontalPadding, metrics::cardVerticalPadding, metrics::cardHorizontalPadding,
                    metrics::cardVerticalPadding});
        setSpacing(metrics::cardSpacing);
    } else {
        setSpacing(0);
    }
    setLayoutHints({.fillWidth = true});
}

void Card::paint(Painter& painter)
{
    const Theme& theme = Theme::current();
    // The window behind is translucent and blurred, so a card drawn solid
    // reads as a different material. It sits a shade clearer than the sidebar,
    // which is what tells the two surfaces apart.
    painter.drawRoundedRect(localRect(), {theme.bigRadius, 0, themeCorners()},
                            theme.secondBackground.withAlpha(kCardOpacity));
}


} // namespace settings
