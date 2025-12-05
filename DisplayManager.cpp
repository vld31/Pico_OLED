#include "DisplayManager.h"
#include "pico/stdlib.h"
#include "QrCode/qrcodegen.hpp"
#include <string.h>

using qrcodegen::QrCode;
using qrcodegen::QrSegment;

uint8_t DisplayManager::i2c_buffer[32];
uint8_t DisplayManager::i2c_len = 0;

DisplayManager::DisplayManager(i2c_inst_t* i2c_port, uint8_t sda_pin, uint8_t scl_pin, uint8_t addr)
    : i2c_port(i2c_port), sda_pin(sda_pin), scl_pin(scl_pin), oled_addr(addr) {
}

DisplayManager::~DisplayManager() {
}

bool DisplayManager::initialize() {
    i2c_init(i2c_port, 400 * 1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_callback, u8x8_gpio_delay_callback);
    u8g2_SetI2CAddress(&u8g2, oled_addr << 1);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    
    return true;
}

void DisplayManager::clear() {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

void DisplayManager::showQRCode(const char* url) {
    drawQRInternal(url);
}

void DisplayManager::showMessage(const char* title, const char* message) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(&u8g2, 2, 10, title);
    
    const int max_chars_per_line = 16;
    int msg_len = strlen(message);
    int y_pos = 25;
    int i = 0;
    
    while (i < msg_len && y_pos < 64) {
        char line[max_chars_per_line + 1];
        int chars_to_take = max_chars_per_line;
        
        if (i + chars_to_take < msg_len) {
            int last_space = -1;
            for (int j = 0; j < chars_to_take; j++) {
                if (message[i + j] == ' ') last_space = j;
            }
            if (last_space > 0) {
                chars_to_take = last_space + 1;
            }
        } else {
            chars_to_take = msg_len - i;
        }
        
        strncpy(line, message + i, chars_to_take);
        line[chars_to_take] = '\0';
        u8g2_DrawStr(&u8g2, 2, y_pos, line);
        y_pos += 12;
        i += chars_to_take;
    }
    
    u8g2_SendBuffer(&u8g2);
}

void DisplayManager::drawQRInternal(const char* text) {
    const QrCode qr = QrCode::encodeText(text, QrCode::Ecc::LOW);
    
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetDrawColor(&u8g2, 1);
    
    const int size = qr.getSize();
    const int scale = 2;
    const int offsetX = (128 - size * scale) / 2;
    const int offsetY = (64 - size * scale) / 2;
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qr.getModule(x, y)) {
                u8g2_DrawBox(&u8g2, offsetX + x * scale, offsetY + y * scale, scale, scale);
            }
        }
    }
    u8g2_SendBuffer(&u8g2);
}

uint8_t DisplayManager::u8x8_byte_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_INIT:
            return 1;
        case U8X8_MSG_BYTE_START_TRANSFER:
            i2c_len = 0;
            return 1;
        case U8X8_MSG_BYTE_SEND: {
            uint8_t *p = (uint8_t *)arg_ptr;
            while (arg_int--) {
                if (i2c_len >= sizeof(i2c_buffer)) return 0;
                i2c_buffer[i2c_len++] = *p++;
            }
            return 1;
        }
        case U8X8_MSG_BYTE_END_TRANSFER: {
            uint8_t addr7 = u8x8_GetI2CAddress(u8x8) >> 1;
            int w = i2c_write_blocking(i2c0, addr7, i2c_buffer, i2c_len, false);
            return (w == (int)i2c_len) ? 1 : 0;
        }
        default:
            return 1;
    }
}

uint8_t DisplayManager::u8x8_gpio_delay_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            return 1;
        case U8X8_MSG_DELAY_MILLI:
            sleep_ms(arg_int);
            return 1;
        case U8X8_MSG_DELAY_10MICRO:
            sleep_us(10);
            return 1;
        case U8X8_MSG_DELAY_100NANO:
            return 1;
        case U8X8_MSG_GPIO_RESET:
            return 1;
        default:
            return 1;
    }
}
