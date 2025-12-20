#pragma once
#include <string>


struct IDisplay {
virtual ~IDisplay() = default;
virtual void clear() = 0;
virtual void drawText(int x, int y, const std::string &text) = 0;
virtual void show() = 0;
};