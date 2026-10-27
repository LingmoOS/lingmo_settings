// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

// The settings pages name the framework clients with lower-case prefixes
// (accounts::, upower::, audio::, ...); the clients themselves live in
// Lingmo::Accounts, Lingmo::UPower, Lingmo::Audio, ... The project's compile
// options force-include this header.
#pragma once

#ifdef __cplusplus

namespace Lingmo {
namespace Accounts {}
namespace Appearance {}
namespace Audio {}
namespace Bluetooth {}
namespace Display {}
namespace Files {}
namespace NetworkManager {}
namespace PowerProfiles {}
namespace TimeDate {}
namespace UPower {}
} // namespace Lingmo

namespace accounts = Lingmo::Accounts;
namespace appearance = Lingmo::Appearance;
namespace audio = Lingmo::Audio;
namespace bluetooth = Lingmo::Bluetooth;
namespace display = Lingmo::Display;
namespace files = Lingmo::Files;
namespace networkmanager = Lingmo::NetworkManager;
namespace powerprofiles = Lingmo::PowerProfiles;
namespace timedate = Lingmo::TimeDate;
namespace upower = Lingmo::UPower;

#endif // __cplusplus
