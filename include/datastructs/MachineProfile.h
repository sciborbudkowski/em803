#pragma once

#include <QString>

enum CPUType {
    I8080,
    I8085,
    I8086,
    I8088,
    Z80,
    NOT_SET
};

class MachineProfile {
    public:
        MachineProfile() = default;
        MachineProfile(CPUType processorType, QString machineName, size_t memorySize) : processorType(processorType), machineName(machineName), memorySize(memorySize) {}

        CPUType getProcessorType() const { return processorType; }
        
        QString getMachineName() const { return machineName; }

        size_t getMemorySize() const { return memorySize; }

        static QString getCPUName(CPUType cpuType) {
            if(cpuType == I8080) return "Intel 8080";
            if(cpuType == I8085) return "Intel 8085";
            if(cpuType == I8086) return "Intel 8086";
            if(cpuType == I8088) return "Intel 8088";
            if(cpuType == Z80) return "Zilog Z80";
            return "Unknown CPU!";
        }

        static CPUType getCPUType(const QString& cpuName) {
            if(cpuName == "Intel 8080") return I8080;
            if(cpuName == "Intel 8085") return I8085;
            if(cpuName == "Intel 8086") return I8086;
            if(cpuName == "Intel 8088") return I8088;
            if(cpuName == "Zilog Z80") return Z80;
            return NOT_SET;
        }

    private:
        CPUType processorType;
        QString machineName;
        size_t memorySize;
};
