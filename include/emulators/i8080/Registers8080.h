#pragma once

#include "IRegisters8080.h"

#include <cstdint>

// namespace i8080 {
//     enum FLAG {
//         CARRY = (1 << 0),
//         PARITY = (1 << 2),
//         AUX_CARRY = (1 << 4),
//         ZERO = (1 << 6),
//         SIGN = (1 << 7)
//     };
// };

struct Registers8080 {
    uint8_t A, B, C, D, E, H, L, F;
    uint16_t SP, PC;

    bool getFlag(i8080::FLAG flag) const { return F & flag; }

    void setFlag(i8080::FLAG flag, bool value) {
        if (value) F |= flag;
        else F &= ~flag;
    }

    void reset() {
        A = B = C = D = E = H = L = F = 0;
        SP = PC = 0;
    }

    uint16_t HL() const { return (H << 8) | L; }
    void HL(uint16_t value) { H = (value >> 8) & 0xFF; L = value & 0xFF; }

    uint16_t BC() const { return (B << 8) | C; }
    void BC(uint16_t value) { B = (value >> 8) & 0xFF; C = value & 0xFF; }

    uint16_t DE() const { return (D << 8) | E; }
    void DE(uint16_t value) { D = (value >> 8) & 0xFF; E = value & 0xFF; }
};
