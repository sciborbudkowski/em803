#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

#include "RaylibWrapper.h"
#include "DejaVuSansMonoFontData.h"

namespace Helpers {

    class AssetLoader {
        public:
            static Font loadFont() {
                try {
                    Font font = LoadFontFromMemory(".ttf", consoleMonoFontData, consoleMonoFontDataSize, 16, nullptr, 256);
                    return font;
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to load embedded font : " + std::string(e.what()));
                }
            }
    };

} // namespace Helpers
