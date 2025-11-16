#ifndef SOUND_H
#define SOUND_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "u8g2.h"

#define BUTTON_PIN 10
#define BUZZER_PIN 20

// External reference to the display object from main
extern u8g2_t u8g2;

void beep(uint freq, uint duration_ms) {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint sys_clock = clock_get_hz(clk_sys);
    uint wrap = sys_clock / freq - 1;
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), (wrap * 3) / 4);  // 75% duty cycle for louder sound

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
}

void sound_check_button() {
    if (gpio_get(BUTTON_PIN)) {
        // Clear display and show message
        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
        u8g2_DrawStr(&u8g2, 10, 30, "Table's are");
        u8g2_DrawStr(&u8g2, 10, 50, "raising!");
        u8g2_SendBuffer(&u8g2);
        
        // Play beep
        beep(1000, 200);
        sleep_ms(200);  // debounce
    }
}

#endif // SOUND_H
