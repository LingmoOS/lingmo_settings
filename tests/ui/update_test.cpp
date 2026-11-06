// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>
#include "base/dbus.h"
#include "base/event_loop.h"
#include "ui/application.h"
#include "ui/button.h"
#include "ui/label.h"
#include "ui/window.h"
#include "window/page.h"
#include "window/settingswindow.h"
#include <chrono>
#include <poll.h>
using namespace Lingmo;
namespace {
ui::View* find(ui::View* root, const std::string& name) {
    if (root->name() == name) return root;
    for (const auto& child : root->children()) if (auto* view = find(child.get(), name)) return view;
    return nullptr;
}
bool wait(const std::function<bool()>& ready) {
    auto& loop = ui::Application::instance()->loop();
    auto end = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (!ready() && std::chrono::steady_clock::now() < end) { pollfd fd{loop.fd(), POLLIN, 0}; ::poll(&fd, 1, 5); loop.dispatch(); }
    return ready();
}
}
TEST(UpdatePage, FailedRequestCanRetryAndUnavailableChannelIsDisabled) {
    settings::SettingsWindow window;
    window.switchToPage("update"); window.reveal(); window.window().grab();
    auto* action = dynamic_cast<ui::Button*>(find(window.currentPage(), "update:action"));
    auto* status = dynamic_cast<ui::Label*>(find(window.currentPage(), "update:status"));
    auto* channel = find(window.currentPage(), "update:channel-combo");
    ASSERT_TRUE(action); ASSERT_TRUE(status); ASSERT_TRUE(channel);
    EXPECT_FALSE(action->isEnabled()); EXPECT_FALSE(channel->isEnabled());
    auto* bus = base::dbus::Connection::session(); ASSERT_TRUE(bus);
    int32_t state = 6; int calls = 0;
    base::dbus::Object object(bus, "/org/lingmo/Services/Update", "org.lingmo.Services.Update");
    object.addProperty("state", [&] { return state; });
    object.addProperty("channel", [] { return int32_t(0); });
    object.addProperty("errorLog", [] { return std::string("Previous failure"); });
    object.addSignal("stateChanged", "i");
    object.addMethod("checkForUpdate", "", "", [&](base::dbus::MethodCall& call) {
        ++calls;
        if (calls == 1) call.replyError("org.lingmo.TestFailure", "Simulated service failure");
        else { state = 1; object.emitSignal("stateChanged", state); call.reply(); }
    });
    ASSERT_TRUE(object.publish()); ASSERT_TRUE(bus->requestName("org.lingmo.Services"));
    ASSERT_TRUE(wait([&] { return action->isEnabled(); }));
    action->onClicked(); EXPECT_FALSE(action->isEnabled());
    ASSERT_TRUE(wait([&] { return calls == 1 && action->isEnabled(); }));
    EXPECT_EQ(status->text(), "Update failed!");
    action->onClicked();
    ASSERT_TRUE(wait([&] { return calls == 2 && status->text() == "Checking for updates"; }));
    EXPECT_FALSE(action->isEnabled()); EXPECT_FALSE(channel->isEnabled());
    bus->releaseName("org.lingmo.Services");
    ASSERT_TRUE(wait([&] { return status->text() == "Update service unavailable"; }));
    EXPECT_FALSE(channel->isEnabled());
}
