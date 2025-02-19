#pragma once

#include <string>
#include <cstring>
#include <algorithm>

namespace Helpers {

    inline std::string toLower(const std::string& str) {
        std::string result(str);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
        return result;
    }

} // namespace Helpers
