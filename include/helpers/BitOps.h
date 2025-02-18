#pragma once

#include <cstdint>

namespace Helpers {

    template <typename T>
    inline bool isBitSet(T number, unsigned int position) {
        return (number >> position) & 1;
    }

    template <typename T>
    inline T setBit(T number, unsigned int position) {
        return number | (1 << position);
    }

} // namespace Helpers
