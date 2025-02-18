#pragma once

#include <cstdint>
#include <vector>

class Memory8080AccessInterface {
    private:
        size_t size;

    public:
        virtual ~Memory8080AccessInterface() = default;

        virtual uint8_t getByte(uint16_t address) const = 0;
        virtual uint16_t getWord(uint16_t address) const = 0;

        virtual void setByte(uint16_t address, uint8_t value) = 0;
        virtual void setWord(uint16_t address, uint16_t value) = 0;

        virtual std::vector<uint8_t> getBytesBlock(uint16_t address, size_t size) const = 0;
        virtual size_t getSize() const { return size; };
};
