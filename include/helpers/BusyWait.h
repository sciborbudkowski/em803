#pragma once

#include <chrono>

namespace Helpers {

    inline void busyWait(std::chrono::microseconds waitDuration) {
        auto start = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start) < waitDuration) {}
    }

} // namespace Helpers
