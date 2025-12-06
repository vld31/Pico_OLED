#ifndef SOUND_CONTROLLER_H
#define SOUND_CONTROLLER_H

#include "pico/stdlib.h"

class SoundController {
public:
    SoundController(uint buzzer_pin);
    ~SoundController();
    
    void initialize();
    void beep(uint frequency_hz = 1000, uint duration_ms = 200);
    
private:
    uint buzzer_pin;
};

#endif // SOUND_CONTROLLER_H
