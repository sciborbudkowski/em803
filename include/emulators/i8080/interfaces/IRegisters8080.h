#pragma once

#include <cstdint>

namespace i8080 {
    enum FLAG {
        CARRY = (1 << 0),
        PARITY = (1 << 2),
        AUX_CARRY = (1 << 4),
        ZERO = (1 << 6),
        SIGN = (1 << 7)
    };
};

class IRegisters8080 {
    public:
        virtual ~IRegisters8080() = default;

        virtual uint8_t A() const = 0;
        virtual uint8_t B() const = 0;
        virtual uint8_t C() const = 0;
        virtual uint8_t D() const = 0;
        virtual uint8_t E() const = 0;
        virtual uint8_t H() const = 0;
        virtual uint8_t L() const = 0;

        virtual uint16_t HL() const = 0;
        virtual uint16_t BC() const = 0;
        virtual uint16_t DE() const = 0;

        virtual uint16_t PC() const = 0;
        virtual uint16_t SP() const = 0;

        virtual bool flag(i8080::FLAG flag) const = 0;
};
