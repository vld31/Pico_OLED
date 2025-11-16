#ifndef SOUND_H
#define SOUND_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "u8g2.h"  // Include u8g2 for display support

#define BUTTON_PIN 14
#define BUZZER_PIN 15

// Declare U8g2 display object
extern u8g2_t u8g2;

void beep(uint freq, uint duration_ms) {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint sys_clock = clock_get_hz(clk_sys);
    uint wrap = sys_clock / freq - 1;
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), wrap / 2);  // 50% duty

    sleep_ms(duration_ms);

    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 0);
}

void sound_init() {
    // Init button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    // Init buzzer
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice, true);

    // Init display (assumes I2C connected and u8g2 is declared elsewhere)
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( 
        &u8g2,
        U8G2_R0,
        u8x8_byte_pico_i2c,
        u8x8_gpio_and_delay_pico
    );
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

void sound_check_button() {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);

    if (gpio_get(BUTTON_PIN)) {
        u8g2_DrawStr(&u8g2, 0, 20, "Table's are raising!");
        beep(1000, 200);
        sleep_ms(200);  // debounce
    } 
    
    u8g2_SendBuffer(&u8g2);
}

#endif // SOUND_H
