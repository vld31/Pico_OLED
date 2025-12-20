#pragma once
#include "IDisplay.h"
#include <vector>
#include <string>

struct MockDisplay : public IDisplay {
    std::vector<std::string> ops;

    void clear() override { ops.push_back("clear"); }

    void drawText(int x, int y, const std::string& text) override {
        ops.push_back("draw:" + std::to_string(x) + ":" + std::to_string(y) + ":" + text);
    }

    void show() override { ops.push_back("show"); }
};
