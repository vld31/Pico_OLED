#include <stdio.h>
#include "pico/stdlib.h"
#include "DisplayManager.h"
#include "WiFiClient.h"
#include "NotificationHandler.h"
#include "SoundController.h"

#define I2C_PORT     i2c0
#define I2C_SDA      4
#define I2C_SCL      5
#define OLED_ADDR    0x3C
#define BUZZER_PIN   20

#define BACKEND_HOST "51.21.129.98"
#define BACKEND_PORT 3000
#define ENDPOINT     "/api/notifications"

DisplayManager* displayMgr = nullptr;

void restoreQRCode() {
    if (displayMgr) {
        displayMgr->showQRCode("https://deskberg.netlify.app/");
    }
}

int main() {
    stdio_init_all();
    
    displayMgr = new DisplayManager(I2C_PORT, I2C_SDA, I2C_SCL, OLED_ADDR);
    if (!displayMgr->initialize()) {
        printf("Display initialization failed\n");
        return 1;
    }
    displayMgr->showQRCode("https://deskberg.netlify.app/");
    
    SoundController soundCtrl(BUZZER_PIN);
    soundCtrl.initialize();
    
    WiFiClient wifi(WIFI_SSID, WIFI_PASSWORD);
    if (!wifi.connect()) {
        return 1;
    }
    
    NotificationHandler notificationHandler(BACKEND_HOST, BACKEND_PORT, ENDPOINT);
    notificationHandler.setDisplayManager(displayMgr);
    notificationHandler.setSoundController(&soundCtrl);
    notificationHandler.setMessageCallback([](const char* msg) {
        sleep_ms(5000);
        restoreQRCode();
    });
    notificationHandler.initialize();
    
    printf("System ready. Polling backend every 5s...\n");
    
    absolute_time_t last_check = get_absolute_time();
    
    while (true) {
        if (absolute_time_diff_us(last_check, get_absolute_time()) > 5000000) {
            notificationHandler.checkNotifications();
            last_check = get_absolute_time();
        }
        
        wifi.poll();
        sleep_ms(10);
    }
    
    delete displayMgr;
    return 0;
}
