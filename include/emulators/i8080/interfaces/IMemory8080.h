#pragma once

#include <cstdint>
#include <vector>

class IMemory8080 {
    private:
        size_t size;

    public:
        virtual ~IMemory8080() = default;

        virtual uint8_t getByte(uint16_t address) const = 0;
        virtual uint16_t getWord(uint16_t address) const = 0;

        virtual void setByte(uint16_t address, uint8_t value) {};
        virtual void setWord(uint16_t address, uint16_t value) {};

        virtual std::vector<uint8_t> getBytesBlock(uint16_t address, size_t size) const = 0;
        virtual size_t getSize() const { return size; };

        virtual bool loadProgram(uint16_t startAddress, std::vector<uint8_t>& buffer) = 0;
};
