#pragma once
#include "IWiFiClient.h"
#include <string>

struct MockWiFiClient : public IWiFiClient {
    bool connect(const std::string& ssid, const std::string& pw) override {
        last_ssid = ssid;
        last_pw = pw;
        return connect_result;
    }

    bool get(const std::string& url, std::string& out_body) override {
        last_url = url;
        out_body = response_body;
        return get_result;
    }

    bool connect_result = true;
    bool get_result = true;
    std::string response_body = R"({"message":"Hello"})";

    std::string last_ssid, last_pw, last_url;
};
