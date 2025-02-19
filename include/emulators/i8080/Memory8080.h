#pragma once

#include "IMemory8080.h"
#include "Calcs.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>

class Memory8080 : public IMemory8080 {
    private:
        size_t size;

        std::vector<uint8_t> memory;
        
    public:
        Memory8080(size_t size = DEFAULT_SIZE) : size(size), memory(size, 0x00) { clear(); };
        ~Memory8080() = default;

        static constexpr size_t DEFAULT_SIZE = 0x10000;

        void clear() { memory = std::vector<uint8_t>(size, 0x00); };

        uint8_t read(uint16_t address) const {
            if(address >= size) {
                throw std::out_of_range("Memory8080::read(uint16_t): address out of range");
            }
            return memory[address];
        }

        void write(uint16_t address, uint8_t value) {
            if(address >= size) {
                throw std::out_of_range("Memory8080::write(uint16_t, uint8_t): address out of range");
            }
            memory[address] = value;
        }

        bool loadProgram(uint16_t address, const std::vector<uint8_t>& program) {
            if(address + program.size() > size) {
                throw std::out_of_range("Memory8080::loadProgram(uint16_t, const std::vector<uint8_t>&): program out of range");
            }
            std::copy(program.begin(), program.end(), memory.begin() + address);
            return true;
        }

        #pragma region Memory8080AccessInterface
        uint8_t getByte(uint16_t address) const override { return memory[address]; }
        void setByte(uint16_t address, uint8_t value) override { memory[address] = value; }

        uint16_t getWord(uint16_t address) const override { return Calcs::makeWord(memory[address], memory[address + 1]); }
        void setWord(uint16_t address, uint16_t value) override { 
            memory[address] = value & 0xFF;
            memory[address + 1] = (value >> 8) & 0xFF;
        }

        std::vector<uint8_t> getBytesBlock(uint16_t address, size_t size) const override {
            if(address >= memory.size()) return {};

            size_t end = std::min(address + size, memory.size());
            return std::vector<uint8_t>(memory.begin() + address, memory.begin() + size);
        }

        size_t getSize() const override { return size; }

        bool loadProgram(uint16_t startAddress, std::vector<uint8_t>& buffer) override {
            if(buffer.size() == 0) return false;
            if(startAddress + buffer.size() > size) return false;

            std::copy(buffer.begin(), buffer.end(), memory.begin() + startAddress);
            return true;
        }
        #pragma endregion
};
