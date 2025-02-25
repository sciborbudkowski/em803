#pragma once

#include <memory>

#include "ICPU.h"
#include "CPU8080.h"
#include "ITerminalAccess.h"

#include <memory>

class CPUFactory {
    public:
        static std::unique_ptr<ICPU> createCPU8080(std::shared_ptr<ITerminalAccess> terminal,  size_t memorySize) {
            std::unique_ptr<CPU8080> cpu = std::make_unique<CPU8080>(terminal, memorySize);
            return cpu;
        }
};
