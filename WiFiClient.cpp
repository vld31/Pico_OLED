#include "WiFiClient.h"
#include <stdio.h>

WiFiClient::WiFiClient(const char* ssid, const char* password)
    : ssid(ssid), password(password), connected(false) {
}

WiFiClient::~WiFiClient() {
    if (connected) {
        cyw43_arch_deinit();
    }
}

bool WiFiClient::connect() {
    printf("Initializing WiFi...\n");
    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    
    printf("Connecting to WiFi: %s\n", ssid);
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, 
        CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("WiFi connection failed\n");
        return false;
    }
    printf("WiFi connected!\n");
    connected = true;
    return true;
}

void WiFiClient::poll() {
    if (connected) {
        cyw43_arch_poll();
    }
}
