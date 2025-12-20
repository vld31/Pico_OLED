#pragma once
#include <string>

#include "IWiFiClient.h"
#include "IDisplay.h"
#include "ISound.h"

class NotificationHandler {
public:
    NotificationHandler(IWiFiClient& wifi, IDisplay& display)
        : wifi_(wifi), display_(display) {}

    void setSound(ISound* s) { sound_ = s; }

    void poll();
    void processMessage(const std::string& message);

private:
    IWiFiClient& wifi_;
    IDisplay& display_;
    ISound* sound_ = nullptr;
};
