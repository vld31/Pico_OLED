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
extern void funcDrawQr();  // Function to redraw QR code

// Modular notification system:
// Call show_notification(message) from anywhere to:
//   1. Display the message on screen
//   2. Play a notification beep
//   3. Auto-restore QR code after 5 seconds

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

void show_notification(const char* message, uint beep_freq = 1000, uint beep_duration = 200) {
    // Clear display and show notification message
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tr);
    
    // Draw notification icon/title
    u8g2_DrawStr(&u8g2, 10, 15, "Notification:");
    
    // Draw the message (simple single line, can be enhanced for multi-line)
    u8g2_DrawStr(&u8g2, 10, 40, message);
    u8g2_SendBuffer(&u8g2);
    
    // Play notification sound
    beep(beep_freq, beep_duration);
    
    // Wait 5 seconds before restoring QR code
    sleep_ms(5000);
    
    // Restore QR code
    funcDrawQr();
}

void sound_check_button() {
    if (gpio_get(BUTTON_PIN)) {
        // Trigger notification when button is pressed (for testing)
        show_notification("Table's raising!");
        
        // Debounce delay
        sleep_ms(200);
    }
}

#endif // SOUND_H
