#pragma once

#include "Calcs.h"

#include <cstdint>

struct ALU8080 {
    enum FLAG {
        CARRY     = (1 << 0),
        PARITY    = (1 << 2),
        AUX_CARRY = (1 << 4),
        ZERO      = (1 << 6),
        SIGN      = (1 << 7)
    };

    static uint8_t add(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint16_t result = acc + value;
        flags = 0;

        if(result & 0x100) flags |= CARRY;
        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(((acc ^ value) & 0x10)) flags |= AUX_CARRY;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;

        return result & 0xFF;
    }

    static uint8_t adc(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t carry = flags & CARRY ? 1 : 0;
        uint16_t result = acc + value + carry;
        flags = 0;

        if(result & 0x100) flags |= CARRY;
        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(((acc ^ value ^ result) & 0x10)) flags |= AUX_CARRY;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;

        return result & 0xFF;
    }

    static uint8_t sub(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint16_t result = acc - value;
        flags = 0;

        if(result & 0x100) flags |= CARRY;
        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(((acc ^ value ^ result) & 0x10)) flags |= AUX_CARRY;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;

        return result & 0xFF;
    }

    static uint8_t sbb(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t carry = flags & CARRY ? 1 : 0;
        uint16_t result = acc - value - carry;
        flags = 0;

        if(result & 0x100) flags |= CARRY;
        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(((acc ^ value ^ result) & 0x10)) flags |= AUX_CARRY;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;

        return result & 0xFF;
    }

    static uint8_t cmp(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t result = sub(acc, value, flags);
        return result;
    }

    static uint8_t and_op(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t result = acc & value;
        flags = 0;

        if(result == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(Calcs::calculateParity(result)) flags |= PARITY;
        flags |= AUX_CARRY;

        return result;
    }

    static uint8_t xor_op(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t result = acc ^ value;
        flags = 0;

        if(result == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(Calcs::calculateParity(result)) flags |= PARITY;

        return result;
    }

    static uint8_t or_op(uint8_t acc, uint8_t value, uint8_t& flags) {
        uint8_t result = acc | value;
        flags = 0;

        if(result == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(Calcs::calculateParity(result)) flags |= PARITY;

        return result;
    }

    static uint8_t inc(uint8_t acc, uint8_t& flags) {
        uint16_t result = acc + 1;

        flags &= ~(ZERO | SIGN | PARITY | AUX_CARRY);

        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;
        if((acc & 0x0F) == 0x0F) flags |= AUX_CARRY;

        return result & 0xFF;
    }

    static uint8_t dec(uint8_t acc, uint8_t& flags) {
        uint16_t result = acc - 1;

        flags &= ~(ZERO | SIGN | PARITY | AUX_CARRY);

        if((result & 0xFF) == 0) flags |= ZERO;
        if(result & 0x80) flags |= SIGN;
        if(Calcs::calculateParity(result & 0xFF)) flags |= PARITY;
        if((acc & 0x0F) == 0x00) flags |= AUX_CARRY;

        return result & 0xFF;
    }
};
