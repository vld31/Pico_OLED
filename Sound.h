#ifndef SOUND_H
#define SOUND_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "u8g2.h"
#include <string.h>

#define BUTTON_PIN 10
#define BUZZER_PIN 20

extern u8g2_t u8g2;
extern void funcDrawQr();  


void beep(uint freq, uint duration_ms) {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint sys_clock = clock_get_hz(clk_sys);
    uint wrap = sys_clock / freq - 1;
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), (wrap * 3) / 4);  

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

void show_notification(const char* message, uint beep_freq = 1000, uint beep_duration = 200) {
    if (!message || !*message) return; // ignore empty

    
    char buf[128];
    strncpy(buf, message, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    int len = (int)strlen(buf);
    while (len > 0 && (buf[len-1] == '\r' || buf[len-1] == '\n' || buf[len-1] == ' ')) {
        buf[--len] = '\0';
    }
    if (len == 0 || strcmp(buf, "OK") == 0) return; 
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
    u8g2_DrawStr(&u8g2, 10, 15, "Notification:");
    u8g2_DrawStr(&u8g2, 10, 40, buf);
    u8g2_SendBuffer(&u8g2);

    beep(beep_freq, beep_duration);

    sleep_ms(5000);

    
    funcDrawQr();
    u8g2_SendBuffer(&u8g2);
}

void sound_check_button() {
    if (gpio_get(BUTTON_PIN)) {
        show_notification("Table's raising!");
        
        sleep_ms(200);
    }
}

#endif 
