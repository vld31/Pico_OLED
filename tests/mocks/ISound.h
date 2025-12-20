#pragma once
#include <string>

struct ISound {
    virtual ~ISound() = default;
    virtual void beep(int frequency, int duration) = 0;
};