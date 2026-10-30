// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pages/dock/docksettings.h"

#include <algorithm>
#include <cstdint>

#include "base/paths.h"
#include "base/settings.h"

using namespace Lingmo::base;

namespace settings {

DockSettings::DockSettings()
    : m_dock(dbus::Connection::session(), "org.lingmo.Dock", "/Dock", "org.lingmo.Dock")
{
    const Settings shell(paths::lingmoConfig("shell"));
    m_iconSize = int(shell.intValue("Dock/IconSize"));
    m_direction = int(shell.intValue("Dock/Direction"));
    m_visibility = int(shell.intValue("Dock/Visibility"));
    m_glassTint = std::clamp(shell.doubleValue("Dock/GlassTint", 0.5), 0.0, 1.0);
    m_minimizedArea = shell.boolValue("Dock/MinimizedArea", true);
    m_magnification = shell.boolValue("Dock/Magnification", true);
    m_magnificationFactor = std::clamp(shell.doubleValue("Dock/MagnificationFactor", 1.5), 1.0, 2.0);
    m_bounceOnLaunch = shell.boolValue("Dock/BounceOnLaunch", true);
}

// Called on every slider move; none of these waits for the dock.
void DockSettings::setIconSize(int size)
{
    if (size == m_iconSize)
        return;
    m_dock.send("setIconSize", int32_t(size));
    m_iconSize = size;
}

void DockSettings::setDirection(int direction)
{
    if (direction == m_direction)
        return;
    m_dock.send("setDirection", int32_t(direction));
    m_direction = direction;
}

void DockSettings::setVisibility(int visibility)
{
    if (visibility == m_visibility)
        return;
    m_dock.send("setVisibility", int32_t(visibility));
    m_visibility = visibility;
}

void DockSettings::setGlassTint(double tint)
{
    tint = std::clamp(tint, 0.0, 1.0);
    if (tint == m_glassTint)
        return;
    m_dock.send("setGlassTint", tint);
    m_glassTint = tint;
}

void DockSettings::setMinimizedArea(bool enabled)
{
    if (enabled == m_minimizedArea)
        return;
    m_dock.send("setMinimizedArea", enabled);
    m_minimizedArea = enabled;
}

void DockSettings::setMagnification(bool enabled)
{
    if (enabled == m_magnification)
        return;
    m_dock.send("setMagnification", enabled);
    m_magnification = enabled;
}

void DockSettings::setMagnificationFactor(double factor)
{
    factor = std::clamp(factor, 1.0, 2.0);
    if (factor == m_magnificationFactor)
        return;
    m_dock.send("setMagnificationFactor", factor);
    m_magnificationFactor = factor;
}

void DockSettings::setBounceOnLaunch(bool enabled)
{
    if (enabled == m_bounceOnLaunch)
        return;
    m_dock.send("setBounceOnLaunch", enabled);
    m_bounceOnLaunch = enabled;
}

} // namespace settings
