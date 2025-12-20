#pragma once
#include <string>


struct IWiFiClient {
virtual ~IWiFiClient() = default;
virtual bool connect(const std::string &ssid, const std::string &pw) = 0;
virtual bool get(const std::string &url, std::string &out_body) = 0;
};