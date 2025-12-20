#include <gtest/gtest.h>

#include "NotificationHandler.h"
#include "mocks/MockWiFiClient.h"
#include "mocks/MockDisplay.h"
#include "mocks/MockSound.h"

TEST(NotificationHandler, ProcessMessageDisplaysAndPlaysSound) {
    MockWiFiClient wifi;
    MockDisplay display;
    MockSound sound;

    NotificationHandler handler(wifi, display);
    handler.setSound(&sound);

    handler.processMessage("Hello");

    ASSERT_GE(display.ops.size(), 4u);
    EXPECT_EQ(display.ops[0], "clear");
    EXPECT_EQ(display.ops[1], "draw:0:0:Notification:");
    EXPECT_EQ(display.ops[2], "draw:0:1:Hello");
    EXPECT_EQ(display.ops.back(), "show");
    EXPECT_TRUE(sound.wasBeeped);
}

TEST(NotificationHandler, PollFetchesFromWifiAndDisplays) {
    MockWiFiClient wifi;
    wifi.response_body = R"([{"title":"Hello","body":"World"}])";

    MockDisplay display;
    MockSound sound;

    NotificationHandler handler(wifi, display);
    handler.setSound(&sound);

    handler.poll();

    EXPECT_FALSE(wifi.last_url.empty());
    ASSERT_GE(display.ops.size(), 3u);
    EXPECT_EQ(display.ops[0], "clear");

    bool saw_title = false, saw_body = false, saw_show = false;
    for (const auto& op : display.ops) {
        if (op.find(":Hello") != std::string::npos) saw_title = true;
        if (op.find(":World") != std::string::npos) saw_body = true;
        if (op == "show") saw_show = true;
    }

    EXPECT_TRUE(saw_title);
    EXPECT_TRUE(saw_body);
    EXPECT_TRUE(saw_show);
    EXPECT_TRUE(sound.wasBeeped);
}
