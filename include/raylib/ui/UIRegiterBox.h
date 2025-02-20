#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <raygui.h>
#include <string>

class UIRegisterBox {
    public:
        static void renderRegistersPanel(float x, float y, std::string label, uint8_t regValue, bool editable = false) {
            int tempValue = static_cast<int>(regValue);
            char registerValue[4];
            snprintf(registerValue, sizeof(registerValue), "%02Xh", tempValue);
            GuiLabel(Rectangle{x, y, 20, 15}, label.c_str());
            GuiTextBox(Rectangle{x + 15, y, 45, 15}, registerValue, sizeof(registerValue), editable);
        }

        static void renderRegistersPanel(float x, float y, std::string label, uint16_t regValue, bool editable = false) {
            int tempValue = static_cast<int>(regValue);
            char registerValue[6];
            snprintf(registerValue, sizeof(registerValue), "%04Xh", tempValue);
            GuiLabel(Rectangle{x, y, 20, 15}, label.c_str());
            GuiTextBox(Rectangle{x + 20, y, 45, 15}, registerValue, sizeof(registerValue), editable);
        }

        static void renderFlagsPanel(float x, float y, std::string label, uint8_t flagValue, bool editable = false) {
            char f[6];
            float boxX = x;
            if(label.length() == 3) boxX -= 5;
            std::cout << "label " << label << " size = " << label.length() << std::endl;
            snprintf(f, sizeof(f), "%d", flagValue);
            GuiLabel(Rectangle{boxX, y, 20, 15}, label.c_str());
            GuiTextBox(Rectangle{x + 15, y, 20, 15}, f, sizeof(f), editable);
        }
};
