#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "hardware/i2c.h"
#include "u8g2.h"

class DisplayManager {
public:
    DisplayManager(i2c_inst_t* i2c_port, uint8_t sda_pin, uint8_t scl_pin, uint8_t addr);
    ~DisplayManager();
    
    bool initialize();
    void clear();
    void showQRCode(const char* url);
    void showMessage(const char* title, const char* message);
    u8g2_t* getU8g2() { return &u8g2; }
    
private:
    i2c_inst_t* i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint8_t oled_addr;
    u8g2_t u8g2;
    
    static uint8_t i2c_buffer[32];
    static uint8_t i2c_len;
    
    static uint8_t u8x8_byte_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
    static uint8_t u8x8_gpio_delay_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
    
    void drawQRInternal(const char* text);
};

#endif 
