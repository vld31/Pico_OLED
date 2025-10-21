#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "u8x8.h"
#include "u8g2.h"
#define I2C_PORT  i2c0
#define I2C_SDA   4
#define I2C_SCL   5
#define OLED_ADDR 0x3C 

static u8g2_t u8g2;

static uint8_t i2c_buffer[32];
static uint8_t i2c_len;
static uint8_t u8x8_byte_pico_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_INIT:
            return 1; 
        case U8X8_MSG_BYTE_START_TRANSFER:
            i2c_len = 0;
            return 1;
        case U8X8_MSG_BYTE_SEND: {
            uint8_t *p = (uint8_t *)arg_ptr;
            while (arg_int--) {
                if (i2c_len >= sizeof i2c_buffer) return 0;
                i2c_buffer[i2c_len++] = *p++;
            }
            return 1;
        }
        case U8X8_MSG_BYTE_END_TRANSFER: {
            uint8_t addr7 = u8x8_GetI2CAddress(u8x8) >> 1;
            int w = i2c_write_blocking(I2C_PORT, addr7, i2c_buffer, i2c_len, false);
            return (w == (int)i2c_len) ? 1 : 0;
        }
        default:
            return 1;
    }
}

static uint8_t u8x8_gpio_and_delay_pico(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: return 1;
        case U8X8_MSG_DELAY_MILLI:         sleep_ms(arg_int); return 1;
        case U8X8_MSG_DELAY_10MICRO:       sleep_us(10);      return 1;
        case U8X8_MSG_DELAY_100NANO:       return 1;
        case U8X8_MSG_GPIO_RESET:          return 1; 
        default:                           return 1;
    }
}
int main() {
    stdio_init_all();
    
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_pico_i2c, u8x8_gpio_and_delay_pico);
    u8g2_SetI2CAddress(&u8g2, OLED_ADDR << 1); 
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf); 
    u8g2_DrawStr(&u8g2, 0, 12, "Hello World");
    u8g2_SendBuffer(&u8g2);
    while (true) {
        sleep_ms(1000);
    }
}