#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <functional>

class WiFiClient {
public:
    WiFiClient(const char* ssid, const char* password);
    ~WiFiClient();
    
    bool connect();
    void poll();
    bool isConnected() const { return connected; }
    
private:
    const char* ssid;
    const char* password;
    bool connected;
};

#endif // WIFI_CLIENT_H
