#ifndef SOUND_H
#define SOUND_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUTTON_PIN 14
#define BUZZER_PIN 15

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
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_enabled(slice, true);
}

void sound_check_button() {
    if (gpio_get(BUTTON_PIN)) {
        beep(1000, 200);
        sleep_ms(200);
    }
}

#endif // SOUND_H
