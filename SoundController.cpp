#include "SoundController.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

SoundController::SoundController(uint buzzer_pin)
    : buzzer_pin(buzzer_pin) {
}

SoundController::~SoundController() {
}

void SoundController::initialize() {
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    pwm_set_enabled(slice_num, false);
}

void SoundController::beep(uint frequency_hz, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t divider = clock_freq / (frequency_hz * 4096);
    
    if (divider < 1) divider = 1;
    if (divider > 255) divider = 255;
    
    pwm_set_clkdiv(slice_num, (float)divider);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_gpio_level(buzzer_pin, 3072); // 75% duty cycle
    pwm_set_enabled(slice_num, true);
    
    sleep_ms(duration_ms);
    
    pwm_set_enabled(slice_num, false);
    pwm_set_gpio_level(buzzer_pin, 0);
}
