#pragma once

#include <cstdint>
#include <string>
#include <iomanip>

class Calcs {
    public:
        static std::string hex8_t[0xFF];
        static std::string hex16_t[0xFFFF];

        static bool calculateParity(uint8_t value) { return __builtin_parity(value); }

        static std::string hex8(uint8_t value) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(value);
            return oss.str();
        }

        static std::string hex16(uint16_t value) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value;
            return oss.str();
        }

        static std::string hex32(uint32_t value) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value;
            return oss.str();
        }

        static std::string hex64(uint64_t value) {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
            return oss.str();
        }

        static std::string bin8(uint8_t value) {
            std::string result;
            for(int i=7; i>=0; --i) {
                result += (value & (1 << i)) ? '1' : '0';
            }
            return result;
        }

        static uint16_t makeWord(uint8_t low, uint8_t high) {
            return (high << 8) | low;
        }

    private:
        Calcs() {
            for(int i=0; i<0xFF; i++) {
                hex8_t[i] = hex8((uint8_t)i);
            }
            for(int i=0; i<0xFFFF; i++) {
                hex16_t[i] = hex16((uint16_t)i);
            }
        }
};
