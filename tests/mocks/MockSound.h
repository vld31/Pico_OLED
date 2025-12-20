#pragma once
#include "ISound.h"

struct MockSound : public ISound {
    bool wasBeeped = false;

    void beep(int, int) override {
        wasBeeped = true;
    }
};
