#pragma once

#include "Memory8080AccessInterface.h"
#include "Calcs.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>

class Memory8080 : public Memory8080AccessInterface {
    public:
        Memory8080(size_t size = 0x10000) : size(size), memory(size, 0x00) { clear(); };
        ~Memory8080() = default;

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

        size_t getSize() const { return size; };

        bool loadProgram(uint16_t address, const std::vector<uint8_t>& program) {
            if(address + program.size() > size) {
                throw std::out_of_range("Memory8080::loadProgram(uint16_t, const std::vector<uint8_t>&): program out of range");
            }
            std::copy(program.begin(), program.end(), memory.begin() + address);
            return true;
        }

        uint8_t getByte(uint16_t address) const override { return memory[address]; }

        uint16_t getWord(uint16_t address) const override { return Calcs::makeWord(memory[address], memory[address + 1]); }

        std::vector<uint8_t> getBytesBlock(uint16_t address, size_t size) const override {
            if(address >= memory.size()) return {};

            size_t end = std::min(address + size, memory.size());
            return std::vector<uint8_t>(memory.begin() + address, memory.begin() + size);
        }

        size_t getSize() const override { return size; }

    private:
        size_t size;

        std::vector<uint8_t> memory;
};
