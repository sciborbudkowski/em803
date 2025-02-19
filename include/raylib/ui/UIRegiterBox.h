#pragma once

#include "RaylibWrapper.h"

#include <cstdint>
#include <iostream>

class UIRegisterBox {
    public:
        static void renderRegistersPanel(float x, float y, const char* label, uint8_t regValue, bool editable = false) {
            int tempValue = static_cast<int>(regValue);
            char registerValue[4];
            snprintf(registerValue, sizeof(registerValue), "%02Xh", tempValue);
            GuiLabel(Rectangle{(float)x, (float)y, 20, 15}, label);
            GuiTextBox(Rectangle{(float)x + 15, (float)y, 40, 15}, registerValue, sizeof(registerValue), editable);
        }

        static void renderRegistersPanel(float x, float y, const char* label, uint16_t regValue, bool editable = false) {
            int tempValue = static_cast<int>(regValue);
            char registerValue[6];
            snprintf(registerValue, sizeof(registerValue), "%04Xh", tempValue);
            GuiLabel(Rectangle{(float)x, (float)y, 20, 15}, label);
            GuiTextBox(Rectangle{(float)x + 20, (float)y, 40, 15}, registerValue, sizeof(registerValue), editable);
        }

        static void renderFlagsPanel(float x, float y, const char* label, uint8_t flagValue, bool editable = false) {
            char f[6];
            snprintf(f, sizeof(f), "%d", flagValue);
            GuiLabel(Rectangle{(float)x, (float)y, 20, 15}, label);
            GuiTextBox(Rectangle{(float)x + 20, (float)y, 40, 15}, f, sizeof(f), editable);
        }
};
