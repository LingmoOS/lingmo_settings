// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "base/signal.h"
#include "lingmo/networkmanager/network_model.h"

namespace settings::network {

// The networks the pages list: NetworkManager's, or the sample networks of
// --mock-network, which have no NetworkManager connection behind them.
class NetworkList
{
public:
    NetworkList();
    ~NetworkList();

    bool isMock() const { return !m_model; }
    const std::vector<Lingmo::NetworkManager::NetworkItem>& items() const;
    // One type of row, sorted for a list.
    std::vector<Lingmo::NetworkManager::NetworkItem> rows(Lingmo::NetworkManager::ConnectionType type) const;
    // The row whose key, connection path or SSID is the value.
    std::optional<Lingmo::NetworkManager::NetworkItem> find(const std::string& connectionPath,
                                                              const std::string& ssid = {}) const;

    Lingmo::base::Signal<> changed;

private:
    std::unique_ptr<Lingmo::NetworkManager::NetworkModel> m_model;
    std::vector<Lingmo::NetworkManager::NetworkItem> m_mock;
    Lingmo::base::Subscription m_subscription;
};

// The sample networks: a connected and a saved Wi-Fi network, two others,
// and two wired connections.
std::vector<Lingmo::NetworkManager::NetworkItem> mockNetworks();

} // namespace settings::network
