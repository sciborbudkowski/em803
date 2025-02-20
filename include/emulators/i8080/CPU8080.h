#pragma once

#include "ALU8080.h"
#include "Calcs.h"
#include "IOPorts8080.h"
#include "Memory8080.h"
#include "Registers8080.h"
#include "ITerminalAccess.h"
#include "Timings8080.h"
#include "IRegisters8080.h"
#include "IMemory8080.h"
#ifdef DEBUG_SHOW_CONSOLE_MESSAGES
#include "BusyWait.h"
#endif

#include <chrono>
#include <cstdint>
#include <deque>
#include <iostream>
#include <vector>

class CPU8080 final : public IRegisters8080, public IMemory8080 {
    private:
        Registers8080 regs;
        Memory8080 mem;
        IOPorts8080 ports;

        bool interruptsEnabled, running, turbo;

        const uint16_t CPM_CBIOS = 0xEA00;
        const uint16_t CPM_BDOS  = 0xDC00;
        const uint16_t CPM_CCP   = 0xD400;
        const uint16_t STACK     = 0xEF00;

        const char* welcomeMessage = "Intel 8080 Emulator\n";

        /* Measure few things, will be disabled later */
        unsigned int executedInstructions = 0;
        int currentTimer = 0, totalSpeeding = 0, lastCycleTime = 0;
        std::deque<std::string> lastIntructions;
        const int lastInstructionsStoredCount = 64;

        void setupMeasure() {
            currentTimer = 0;
            executedInstructions = 0;
            totalSpeeding = 0;
            lastCycleTime = 0;
            lastIntructions.clear();
        }

    public:
        CPU8080(ITerminalAccess& terminal, size_t memorySize = Memory8080::DEFAULT_SIZE)
        : ports(&terminal), mem(memorySize) {
            interruptsEnabled = false;
            running = false;
            turbo = false;
        }
        ~CPU8080() = default;

        bool isRunning() const { return running; }
        void start() {
            running = true;
            while(running) step();
        }
        void stop() { running = false; }

        void reset() {
            regs.reset();
            mem.clear();
            ports.reset();
            ports.outString(welcomeMessage);
            running = false;
            turbo = false;

            setupMeasure();
        }

        bool isTurbo() const { return turbo; }
        void setTurbo(bool value) { turbo = value; }

        void step() {
            if(!running) return;

            uint16_t opcode = mem.read(regs.PC++);
            decodeAndExecute(opcode);
        }

        void run() {
            running = true;
            while(running) {
                step();
            }
        }

        void push(int16_t value) {
            mem.write(--regs.SP, (value >> 8) & 0xFF);
            mem.write(--regs.SP, value & 0xFF);
        }

        uint16_t pop() {
            regs.SP += 2;
            return Calcs::makeWord(mem.read(regs.SP), mem.read(regs.SP+1));
        }

        void decodeAndExecute(uint16_t opcode) {
            int timeInst = Timings8080::instructionTimes[opcode];
            std::string asmInst = "";

            auto startTime = std::chrono::high_resolution_clock::now();

            switch(opcode) {
                case 0x00: asmInst = "NOP"; break;

                // --- MOV r1, r2 ---
                case 0x78: regs.A = regs.B; asmInst = "MOV A, B"; break;
                case 0x79: regs.A = regs.C; asmInst = "MOV A, C"; break;
                case 0x7A: regs.A = regs.D; asmInst = "MOV A, D"; break;
                case 0x7B: regs.A = regs.E; asmInst = "MOV A, E"; break;
                case 0x7C: regs.A = regs.H; asmInst = "MOV A, H"; break;
                case 0x7D: regs.A = regs.L; asmInst = "MOV A, L"; break;
                case 0x7E: regs.A = mem.read(regs.HL()); asmInst = "MOV A, [" + Calcs::hex16(regs.HL()) + "]"; break;
                case 0x7F: regs.A = regs.A; asmInst = "MOV A, A"; break;
                
                case 0x40: regs.B = regs.B; asmInst = "MOV B, B"; break;
                case 0x41: regs.B = regs.C; asmInst = "MOV B, C"; break;
                case 0x42: regs.B = regs.D; asmInst = "MOV B, D"; break;
                case 0x43: regs.B = regs.E; asmInst = "MOV B, E"; break;
                case 0x44: regs.B = regs.H; asmInst = "MOV B, H"; break;
                case 0x45: regs.B = regs.L; asmInst = "MOV B, L"; break;
                case 0x46: regs.B = mem.read(regs.HL()); asmInst = "MOV B, [" + Calcs::hex16(regs.HL()) + "]"; break;
                case 0x47: regs.B = regs.A; asmInst = "MOV B, A"; break;

                case 0x48: regs.C = regs.B; asmInst = "MOV C, B"; break;
                case 0x49: regs.C = regs.C; asmInst = "MOV C, C"; break;
                case 0x4A: regs.C = regs.D; asmInst = "MOV C, D"; break;
                case 0x4B: regs.C = regs.E; asmInst = "MOV C, E"; break;
                case 0x4C: regs.C = regs.H; asmInst = "MOV C, H"; break;
                case 0x4D: regs.C = regs.L; asmInst = "MOV C, L"; break;
                case 0x4E: regs.C = mem.read(regs.HL()); asmInst = "MOV C, [" + Calcs::hex16(regs.HL()) + "]"; break;
                case 0x4F: regs.C = regs.A; asmInst = "MOV C, A"; break;
                
                // --- MVI r, data ---
                case 0x06: regs.B = mem.read(regs.PC++); asmInst = "MVI B, " + Calcs::hex8(regs.B); break;
                case 0x0E: regs.C = mem.read(regs.PC++); asmInst = "MVI C, " + Calcs::hex8(regs.C); break;
                case 0x16: regs.D = mem.read(regs.PC++); asmInst = "MVI D, " + Calcs::hex8(regs.D); break;
                case 0x1E: regs.E = mem.read(regs.PC++); asmInst = "MVI E, " + Calcs::hex8(regs.E); break;
                case 0x26: regs.H = mem.read(regs.PC++); asmInst = "MVI H, " + Calcs::hex8(regs.H); break;
                case 0x2E: regs.L = mem.read(regs.PC++); asmInst = "MVI L, " + Calcs::hex8(regs.L); break;
                case 0x3E: regs.A = mem.read(regs.PC++); asmInst = "MVI A, " + Calcs::hex8(regs.A); break;

                // --- ADD A, r ---
                case 0x80: ALU8080::add(regs.A, regs.B, regs.F); asmInst = "ADD B"; break;
                case 0x81: ALU8080::add(regs.A, regs.C, regs.F); asmInst = "ADD C"; break;
                case 0x82: ALU8080::add(regs.A, regs.D, regs.F); asmInst = "ADD D"; break;
                case 0x83: ALU8080::add(regs.A, regs.E, regs.F); asmInst = "ADD E"; break;
                case 0x84: ALU8080::add(regs.A, regs.H, regs.F); asmInst = "ADD H"; break;
                case 0x85: ALU8080::add(regs.A, regs.L, regs.F); asmInst = "ADD L"; break;
                case 0x86: ALU8080::add(regs.A, mem.read(regs.HL()), regs.F); asmInst = "ADD M"; break;
                case 0x87: ALU8080::add(regs.A, regs.A, regs.F); asmInst = "ADD A"; break;

                // --- SUB A, r ---
                case 0x90: ALU8080::sub(regs.A, regs.B, regs.F); asmInst = "SUB B"; break;
                case 0x91: ALU8080::sub(regs.A, regs.C, regs.F); asmInst = "SUB C"; break;
                case 0x92: ALU8080::sub(regs.A, regs.D, regs.F); asmInst = "SUB D"; break;
                case 0x93: ALU8080::sub(regs.A, regs.E, regs.F); asmInst = "SUB E"; break;
                case 0x94: ALU8080::sub(regs.A, regs.H, regs.F); asmInst = "SUB H"; break;
                case 0x95: ALU8080::sub(regs.A, regs.L, regs.F); asmInst = "SUB L"; break;
                case 0x96: ALU8080::sub(regs.A, mem.read(regs.HL()), regs.F); asmInst = "SUB M"; break;
                case 0x97: ALU8080::sub(regs.A, regs.A, regs.F); asmInst = "SUB A"; break;

                // --- INR r ---
                case 0x04: ALU8080::inc(regs.B, regs.F); asmInst = "INR B"; break;
                case 0x0C: ALU8080::inc(regs.C, regs.F); asmInst = "INR C"; break;
                case 0x14: ALU8080::inc(regs.D, regs.F); asmInst = "INR D"; break;
                case 0x1C: ALU8080::inc(regs.E, regs.F); asmInst = "INR E"; break;
                case 0x24: ALU8080::inc(regs.H, regs.F); asmInst = "INR H"; break;
                case 0x2C: ALU8080::inc(regs.L, regs.F); asmInst = "INR L"; break;
                case 0x3C: ALU8080::inc(regs.A, regs.F); asmInst = "INR A"; break;

                // --- JMP addr ---
                case 0xC3: 
                    regs.PC = Calcs::makeWord(mem.read(regs.PC), mem.read(regs.PC + 1));
                    asmInst = "JMP " + Calcs::hex16(regs.PC);
                    break;

                // --- PUSH rp ---
                case 0xC5: push(regs.BC()); asmInst = "PUSH B"; break;
                case 0xD5: push(regs.DE()); asmInst = "PUSH D"; break;
                case 0xE5: push(regs.HL()); asmInst = "PUSH H"; break;
                case 0xF5: {
                    uint8_t flags = regs.F | 0b00000010;
                    push((regs.A << 8) | flags);
                    asmInst = "PUSH PSW";
                    break;
                }

                // --- POP rp ---
                case 0xC1: {
                        uint16_t value = pop();
                        regs.B = (value >> 8) & 0xFF;
                        regs.C = value & 0xFF;
                        asmInst = "POP B";
                        break;
                    }
                case 0xD1: {
                        uint16_t value = pop();
                        regs.D = (value >> 8) & 0xFF;
                        regs.E = value & 0xFF;
                        asmInst = "POP D";
                        break;
                    }
                case 0xE1: {
                        uint16_t value = pop();
                        regs.H = (value >> 8) & 0xFF;
                        regs.L = value & 0xFF;
                        asmInst = "POP H";
                        break;
                    }

                // POP PSW
                case 0xF1: {
                        uint16_t value = pop();
                        regs.setFlag(i8080::FLAG::AUX_CARRY, (value >> 8) & 0xFF);
                        regs.setFlag(i8080::FLAG::ZERO, (value >> 6) & 1);
                        regs.setFlag(i8080::FLAG::SIGN, (value >> 7) & 1);
                        regs.setFlag(i8080::FLAG::PARITY, (value >> 2) & 1);
                        regs.setFlag(i8080::FLAG::CARRY, value & 1);
                        asmInst = "POP PSW";
                        break;
                    }

                // --- CALL addr ---
                case 0xCD: {
                        uint16_t addr = Calcs::makeWord(mem.read(regs.PC), mem.read(regs.PC + 1));
                        regs.PC += 2;

                        if(addr == CPM_CBIOS) {
                            // if(cpmbios) {
                            // cpmbios.call(static_cast<BIOSCALL>(regs.C));
                            // } else {
                            //     throw std::runtime_error("CPM BIOS not initialized!");
                            // }
                            asmInst = "CALL BIOS " + Calcs::hex8(regs.C);
                        } else {
                            push(regs.PC);
                            regs.PC = addr;
                            asmInst = "CALL [" + Calcs::hex16(addr) + "]";
                        }
                        break;
                    }

                // --- RET ---
                case 0xC9: regs.PC = pop(); asmInst = "RET"; break;

                // --- conditional jumps ---
                case 0xCA: { // JZ addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC + 1) << 8);
                    regs.PC += 2;
                    if(regs.getFlag(i8080::ZERO)) {
                        regs.PC = addr;
                    }
                    asmInst = "JZ [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xC2: { // JNZ addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::ZERO)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JNZ [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xD2: { // JNC addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::CARRY)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JNC [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xDA: { // JC addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::CARRY)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JC [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xE2: { // JPO addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::PARITY)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JPO [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xEA: { // JPE addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::PARITY)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JPE [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xFA: { // JM addr
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::SIGN)) {
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "JM [" + Calcs::hex16(addr) + "]";
                    break;
                }

                // ---conditional calls ---
                case 0xCC: { // CZ
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::ZERO)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CZ [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xC4: { // CNZ
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::ZERO)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CNZ [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xDC: { // CC
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::CARRY)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CC [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xD4: { // CNC
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::CARRY)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CNC [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xEC: { // CPE
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::PARITY)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CPE [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xE4: { // CPO
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::PARITY)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CPO [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xF4: { // CP
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(!regs.getFlag(i8080::SIGN)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CP [" + Calcs::hex16(addr) + "]";
                    break;
                }

                case 0xFC: { // CM
                    uint16_t addr = mem.read(regs.PC) | (mem.read(regs.PC+1) << 8);
                    if(regs.getFlag(i8080::SIGN)) {
                        regs.PC += 2;
                        push(regs.PC);
                        regs.PC = addr;
                    } else {
                        regs.PC += 2;
                    }
                    asmInst = "CM [" + Calcs::hex16(addr) + "]";
                    break;
                }

                // --- AND (ANA) ---
                case 0xA0: regs.B = ALU8080::and_op(regs.A, regs.B, regs.F); asmInst = "ANA B"; break;
                case 0xA1: regs.C = ALU8080::and_op(regs.A, regs.C, regs.F); asmInst = "ANA C"; break;
                case 0xA2: regs.D = ALU8080::and_op(regs.A, regs.D, regs.F); asmInst = "ANA D"; break;
                case 0xA3: regs.E = ALU8080::and_op(regs.A, regs.E, regs.F); asmInst = "ANA E"; break;
                case 0xA4: regs.H = ALU8080::and_op(regs.A, regs.H, regs.F); asmInst = "ANA H"; break;
                case 0xA5: regs.L = ALU8080::and_op(regs.A, regs.L, regs.F); asmInst = "ANA L"; break;
                case 0xA6: regs.A = ALU8080::and_op(regs.A, mem.read(regs.HL()), regs.F); asmInst = "ANA M"; break;
                case 0xA7: regs.A = ALU8080::and_op(regs.A, regs.A, regs.F); asmInst = "ANA A"; break;

                // -- AND immediate (ANI) ---
                case 0xE6: {
                    uint8_t value = mem.read(regs.PC++);
                    regs.A &= value;
                    ALU8080::and_op(regs.A, value, regs.F);
                    asmInst = "ANI " + Calcs::hex8(value);
                    break;
                }

                // --- XOR (XRA) ---
                case 0xA8: regs.B = ALU8080::xor_op(regs.A, regs.B, regs.F); asmInst = "XRA B"; break;
                case 0xA9: regs.C = ALU8080::xor_op(regs.A, regs.C, regs.F); asmInst = "XRA C"; break;
                case 0xAA: regs.D = ALU8080::xor_op(regs.A, regs.D, regs.F); asmInst = "XRA D"; break;
                case 0xAB: regs.E = ALU8080::xor_op(regs.A, regs.E, regs.F); asmInst = "XRA E"; break;
                case 0xAC: regs.H = ALU8080::xor_op(regs.A, regs.H, regs.F); asmInst = "XRA H"; break;
                case 0xAD: regs.L = ALU8080::xor_op(regs.A, regs.L, regs.F); asmInst = "XRA L"; break;
                case 0xAE: regs.A = ALU8080::xor_op(regs.A, mem.read(regs.HL()), regs.F); asmInst = "XRA M"; break;
                case 0xAF: regs.A = ALU8080::xor_op(regs.A, regs.A, regs.F); asmInst = "XRA A"; break;

                // --- XOR immediate (XRI) ---
                case 0xEE: {
                    uint8_t value = mem.read(regs.PC++);
                    regs.A ^= value;
                    ALU8080::xor_op(regs.A, value, regs.F);
                    asmInst = "XRI " + Calcs::hex8(value);
                    break;
                }

                // --- OR (ORA) ---
                case 0xB0: regs.B = ALU8080::or_op(regs.A, regs.B, regs.F); asmInst = "ORA B"; break;
                case 0xB1: regs.C = ALU8080::or_op(regs.A, regs.C, regs.F); asmInst = "ORA C"; break;
                case 0xB2: regs.D = ALU8080::or_op(regs.A, regs.D, regs.F); asmInst = "ORA D"; break;
                case 0xB3: regs.E = ALU8080::or_op(regs.A, regs.E, regs.F); asmInst = "ORA E"; break;
                case 0xB4: regs.H = ALU8080::or_op(regs.A, regs.H, regs.F); asmInst = "ORA H"; break;
                case 0xB5: regs.L = ALU8080::or_op(regs.A, regs.L, regs.F); asmInst = "ORA L"; break;
                case 0xB6: regs.A = ALU8080::or_op(regs.A, mem.read(regs.HL()), regs.F); asmInst = "ORA M"; break;
                case 0xB7: regs.A = ALU8080::or_op(regs.A, regs.A, regs.F); asmInst = "ORA A"; break;

                // --- OR immediate (ORI) ---
                case 0xF6: {
                    uint8_t value = mem.read(regs.PC++);
                    regs.A |= value;
                    ALU8080::or_op(regs.A, value, regs.F);
                    asmInst = "ORI " + Calcs::hex8(value);
                    break;
                }

                // --- CPI (Compare A with immediate) ---
                case 0xFE: {
                    uint8_t data = mem.read(regs.PC++);
                    uint8_t flags = regs.F;
                    ALU8080::cmp(regs.A, data, flags);
                    regs.F = flags;
                    asmInst = "CPI " + Calcs::hex8(data);
                    break;
                }

                // --- IN port ---
                case 0xDB: {
                    uint8_t port = mem.read(regs.PC++);
                    uint8_t value = ports.in(port);
                    regs.A = value;
                    asmInst = "IN " + Calcs::hex8(port);
                    break;
                }

                // --- OUT port ---
                case 0xD3: {
                    uint8_t port = mem.read(regs.PC++);
                    ports.out(port, regs.A);
                    asmInst = "OUT " + Calcs::hex8(port) + ", " + Calcs::hex8(regs.A);
                    break;
                }

                // --- HLT ---
                case 0x76: {
                    running = false;
                    asmInst = "HLT"; 
                    std::cout << "Number of executed instructions: " << executedInstructions << std::endl; 
                    std::cout << "Total speeding: " << totalSpeeding << "us" << std::endl;
                    break;
                }

                // --- LXI B, d16 ---
                case 0x01:
                    regs.C = mem.read(regs.PC++);
                    regs.B = mem.read(regs.PC++);
                    asmInst = "LXI B, " + Calcs::hex16((regs.B << 8) | regs.C);
                    break;

                // --- STAX B ---
                case 0x02: mem.write(regs.BC(), regs.A); asmInst = "STAX B"; break;

                // --- INX B ---
                case 0x03:
                    regs.C++;
                    if(regs.C == 0) regs.B++;
                    asmInst = "INX B";
                    break;

                // --- DCR B ---
                case 0x05: ALU8080::dec(regs.B, regs.F); asmInst = "DCR B"; break;

                // -- RLC --
                case 0x07:
                    regs.setFlag(i8080::CARRY, (regs.A & 0x80) != 0);
                    regs.A = (regs.A << 1) | regs.getFlag(i8080::CARRY);
                    asmInst = "RLC";
                    break;

                // --- DAD B (HL += BC) ---
                case 0x09: {
                        uint32_t result = regs.HL() + regs.BC();
                        regs.H = (result >> 8) & 0xFF;
                        regs.L = result & 0xFF;
                        regs.setFlag(i8080::CARRY, result > 0xFFFF);
                        asmInst = "DAD B";
                    }
                    break;

                // --- LDAX B ---
                case 0x0A: regs.A = mem.read(regs.BC()); asmInst = "LDAX B"; break;

                // --- DCX B ---
                case 0x0B: regs.BC(regs.BC() - 1); asmInst = "DCX B"; break;

                // --- DCR C ---
                case 0x0D: {
                    uint8_t flags = regs.F;
                    regs.C = ALU8080::dec(regs.C, flags);
                    regs.F = flags;
                    asmInst = "DCR C";
                    break;
                }

                // --- RRC ---
                case 0x0F:
                    regs.setFlag(i8080::CARRY, (regs.A & 1) != 0);
                    regs.A = (regs.A >> 1) | (regs.getFlag(i8080::CARRY) << 7);
                    asmInst = "RRC";
                    break;

                // --- LXI D, d16 ---
                case 0x11:
                    regs.E = mem.read(regs.PC++);
                    regs.D = mem.read(regs.PC++);
                    asmInst = "LXI D, " + Calcs::hex16(regs.DE());
                    break;

                // --- STAX D ---
                case 0x12: mem.write(regs.DE(), regs.A); asmInst = "STAX D"; break;

                // --- INX D ---
                case 0x13:
                    regs.E++;
                    if(regs.E == 0) regs.D++;
                    asmInst = "INX D";
                    break;

                // --- DCR D ---
                case 0x15: regs.D--; ALU8080::dec(regs.D, regs.F); asmInst = "DCR D"; break;

                // --- RAL ---
                case 0x17: {
                        uint8_t oldCY = regs.getFlag(i8080::CARRY);
                        regs.setFlag(i8080::CARRY, (regs.A & 0x80) != 0);
                        regs.A = (regs.A << 1) | oldCY;
                        asmInst = "RAL";
                        break;
                    }

                // --- DAD D ---
                case 0x19: {
                        uint32_t result = regs.HL() + regs.DE();
                        regs.H = (result >> 8) & 0xFF;
                        regs.L = result & 0xFF;
                        regs.setFlag(i8080::CARRY, result > 0XFFFF);
                        asmInst = "DAD D";
                        break;
                    }

                // --- LDAX D ---
                case 0x1A: regs.A = mem.read(regs.DE()); asmInst = "LDAX D"; break;

                // --- DCX D ---
                case 0x1B:
                    regs.E--;
                    if(regs.E == 0xFF) regs.D--;
                    asmInst = "DCX D";
                    break;

                // --- DCR E ---
                case 0x1D: regs.E--; ALU8080::dec(regs.E, regs.F); asmInst = "DCR E"; break;

                // --- RAR ---
                case 0x1F: {
                        uint8_t oldCY = regs.getFlag(i8080::CARRY);
                        regs.setFlag(i8080::CARRY, (regs.A & 1) != 0);
                        regs.A = (regs.A >> 1) | (oldCY << 7);
                        asmInst = "RAR";
                        break;
                    }

                // --- LXI H, d16 ---
                case 0x21:
                    regs.L = mem.read(regs.PC);
                    regs.H = mem.read(regs.PC+1);
                    regs.PC += 2;
                    asmInst = "LXI H, " + Calcs::hex16(regs.HL());
                    break;

                // --- SHLD addr ---
                case 0x22: {
                        uint8_t low = mem.read(regs.PC++);
                        uint8_t high = mem.read(regs.PC++);
                        uint16_t addr = (high << 8) | low;
                        mem.write(addr, regs.L);
                        mem.write(addr+1, regs.H);
                        asmInst = "SHLD " + Calcs::hex16(addr);
                        break;
                    }

                // --- INX H ---
                case 0x23:
                    regs.L++;
                    if(regs.L == 0) regs.H++;
                    asmInst = "INX H";
                    break;

                // --- DRC H ---
                case 0x25: regs.H--; ALU8080::dec(regs.H, regs.F); asmInst = "DCR H"; break;

                // --- DAA ---
                case 0x27: {
                        if((regs.A & 0x0F) > 9 || regs.getFlag(i8080::AUX_CARRY)) {
                            regs.A += 6;
                            regs.setFlag(i8080::AUX_CARRY, 1);
                        }
                        if((regs.A >> 4) > 9 || regs.getFlag(i8080::CARRY)) {
                            regs.A += 0x60;
                            regs.setFlag(i8080::CARRY, 1);
                        }
                        asmInst = "DAA";
                        break;
                    }

                case 0x50: regs.D = regs.B; asmInst = "MOV D, B"; break;
                case 0x51: regs.D = regs.C; asmInst = "MOV D, C"; break;
                case 0x52: regs.D = regs.D; asmInst = "MOV D, D"; break;
                case 0x53: regs.D = regs.E; asmInst = "MOV D, E"; break;
                case 0x54: regs.D = regs.H; asmInst = "MOV D, H"; break;
                case 0x55: regs.D = regs.L; asmInst = "MOV D, L"; break;
                case 0x56: regs.D = mem.read(regs.HL()); asmInst = "MOV D, M"; break;
                case 0x57: regs.D = regs.A; asmInst = "MOV D, A"; break;

                case 0x58: regs.E = regs.B; asmInst = "MOV E, B"; break;
                case 0x59: regs.E = regs.C; asmInst = "MOV E, C"; break;
                case 0x5A: regs.E = regs.D; asmInst = "MOV E, D"; break;
                case 0x5B: regs.E = regs.E; asmInst = "MOV E, E"; break;
                case 0x5C: regs.E = regs.H; asmInst = "MOV E, H"; break;
                case 0x5D: regs.E = regs.L; asmInst = "MOV E, L"; break;
                case 0x5E: regs.E = mem.read(regs.HL()); asmInst = "MOV E, M"; break;
                case 0x5F: regs.E = regs.A; asmInst = "MOV E, A"; break;

                case 0x60: regs.H = regs.B; asmInst = "MOV H, B"; break;
                case 0x61: regs.H = regs.C; asmInst = "MOV H, C"; break;
                case 0x62: regs.H = regs.D; asmInst = "MOV H, D"; break;
                case 0x63: regs.H = regs.E; asmInst = "MOV H, E"; break;
                case 0x64: regs.H = regs.H; asmInst = "MOV H, H"; break;
                case 0x65: regs.H = regs.L; asmInst = "MOV H, L"; break;
                case 0x66: regs.H = mem.read(regs.HL()); asmInst = "MOV H, M"; break;
                case 0x67: regs.H = regs.A; asmInst = "MOV H, A"; break;

                case 0x68: regs.L = regs.B; asmInst = "MOV L, B"; break;
                case 0x69: regs.L = regs.C; asmInst = "MOV L, C"; break;
                case 0x6A: regs.L = regs.D; asmInst = "MOV L, D"; break;
                case 0x6B: regs.L = regs.E; asmInst = "MOV L, E"; break;
                case 0x6C: regs.L = regs.H; asmInst = "MOV L, H"; break;
                case 0x6D: regs.L = regs.L; asmInst = "MOV L, L"; break;
                case 0x6E: regs.L = mem.read(regs.HL()); asmInst = "MOV L, M"; break;
                case 0x6F: regs.L = regs.A; asmInst = "MOV L, A"; break;

                case 0x70: mem.write(regs.HL(), regs.B); asmInst = "MOV M, B"; break;
                case 0x71: mem.write(regs.HL(), regs.C); asmInst = "MOV M, C"; break;
                case 0x72: mem.write(regs.HL(), regs.D); asmInst = "MOV M, D"; break;
                case 0x73: mem.write(regs.HL(), regs.E); asmInst = "MOV M, E"; break;
                case 0x74: mem.write(regs.HL(), regs.H); asmInst = "MOV M, H"; break;
                case 0x75: mem.write(regs.HL(), regs.L); asmInst = "MOV M, L"; break;

                case 0x28: asmInst = "NOP 0x28 (reserved)"; break;
                case 0x30: asmInst = "NOP 0x30 (reserved)"; break;
                case 0x38: asmInst = "NOP 0x38 (reserved)"; break;
                case 0x08: asmInst = "NOP 0x08 (reserved)"; break;
                case 0x10: asmInst = "NOP 0x10 (reserved)"; break;
                case 0x18: asmInst = "NOP 0x18 (reserved)"; break;
                case 0x20: asmInst = "NOP 0x20 (reserved)"; break;

                // --- DAD H (HL = HL + HL)
                case 0x29: {
                        uint32_t result = regs.HL() + regs.HL();
                        regs.H = (result >> 8) & 0xFF;
                        regs.L = result & 0xFF;
                        regs.setFlag(i8080::CARRY, result > 0xFFFF);
                        asmInst = "DAD H";
                        break;
                    }

                // --- LHLD addr ---
                case 0x2a: {
                        uint8_t low = mem.read(regs.PC++);
                        uint8_t high = mem.read(regs.PC++);
                        uint16_t addr =  (high << 8) | low;
                        regs.L = mem.read(addr);
                        regs.H = mem.read(addr+1);
                        asmInst = "LHLD " + Calcs::hex16(addr);
                        break;
                    }

                // --- DCX H (HL = HL - ) ---
                case 0x2b:
                    if(regs.L == 0) regs.H--;
                    regs.L--;
                    asmInst = "DCX H";
                    break;

                // --- DCR L ---
                case 0x2d: regs.L--; ALU8080::dec(regs.L, regs.F); asmInst = "DCR L"; break;

                // --- CMA (A = ~A)
                case 0x2f: regs.A = ~regs.A; asmInst = "CMA"; break;

                // --- LXI SP, d16 ---
                case 0x31: {
                        uint8_t low = mem.read(regs.PC++);
                        uint8_t high = mem.read(regs.PC++);
                        regs.SP = Calcs::makeWord(high, low);
                        asmInst = "LXI SP, " + Calcs::hex16(regs.SP);
                        break;
                    }


                // --- STA addr ---
                case 0x32: {
                        uint8_t low = mem.read(regs.PC++);
                        uint8_t high = mem.read(regs.PC++);
                        uint16_t addr = Calcs::makeWord(high, low);
                        mem.write(addr, regs.A);
                        asmInst = "STA " + Calcs::hex16(addr);
                        break;
                    }

                // --- INX SP ---
                case 0x33: regs.SP++; asmInst = "INX SP"; break;

                // --- INR M ---
                case 0x34: {
                        uint16_t addr = regs.HL();
                        uint8_t value = mem.read(addr);
                        value = ALU8080::inc(value, regs.F);
                        mem.write(addr, value);
                        asmInst = "INR M";
                    }
                    break;

                // --- DCR M ---
                case 0x35: {
                        uint16_t addr = regs.HL();
                        uint8_t value = mem.read(addr);
                        mem.write(addr, value--);
                        ALU8080::dec(value, regs.F);
                        asmInst = "DCR M";
                        break;
                    }

                // --- MVI M, d8 ---
                case 0x36: {
                        uint8_t value = mem.read(regs.PC++);
                        mem.write(regs.HL(), value);
                        asmInst = "MVI M, " + Calcs::hex8(value);
                        break;
                    }

                // --- STC ---
                case 0x37: regs.setFlag(i8080::CARRY, 1); asmInst = "STC"; break;

                // --- DAD SP ---
                case 0x39: {
                        uint32_t result = regs.HL() + regs.SP;
                        regs.H = (result >> 8) & 0xFF;
                        regs.L = result & 0xFF;
                        regs.setFlag(i8080::CARRY, result > 0xFFFF);
                        asmInst = "DAD SP";
                        break;
                    }

                // --- LDA addr ---
                case 0x3a: {
                        uint8_t low = mem.read(regs.PC++);
                        uint8_t high = mem.read(regs.PC++);
                        uint16_t addr = Calcs::makeWord(high, low);
                        regs.A = mem.read(addr);
                        asmInst = "LDA " + Calcs::hex16(addr);
                        break;
                    }

                // --- DCX SP ---
                case 0x3b: regs.SP--; asmInst = "DCX SP"; break;

                // --- DCR A ---
                case 0x3d: regs.A--; ALU8080::dec(regs.A, regs.F); asmInst = "DCR A"; break;

                // --- CMC ---
                case 0x3f: regs.setFlag(i8080::CARRY, !regs.getFlag(i8080::CARRY)); asmInst = "CMC"; break;

                // --- MOV M, A ---
                case 0x77: mem.write(regs.HL(), regs.A); asmInst = "MOV M, A"; break;

                // --- ADC reg ---
                case 0x88: regs.A = ALU8080::adc(regs.A, regs.B, regs.F); asmInst = "ADC B"; break;
                case 0x89: regs.A = ALU8080::adc(regs.A, regs.C, regs.F); asmInst = "ADC C"; break;
                case 0x8a: regs.A = ALU8080::adc(regs.A, regs.D, regs.F); asmInst = "ADC D"; break;
                case 0x8b: regs.A = ALU8080::adc(regs.A, regs.E, regs.F); asmInst = "ADC E"; break;
                case 0x8c: regs.A = ALU8080::adc(regs.A, regs.H, regs.F); asmInst = "ADC H"; break;
                case 0x8d: regs.A = ALU8080::adc(regs.A, regs.L, regs.F); asmInst = "ADC L"; break;
                case 0x8e: regs.A = ALU8080::adc(regs.A, mem.read(regs.HL()), regs.F); asmInst = "ADC M"; break;
                case 0x8f: regs.A = ALU8080::adc(regs.A, regs.A, regs.F); asmInst = "ADC A"; break;

                // --- SBB reg ---
                case 0x98: regs.A = ALU8080::sbb(regs.A, regs.B, regs.F); asmInst = "SBB B"; break;
                case 0x99: regs.A = ALU8080::sbb(regs.A, regs.C, regs.F); asmInst = "SBB C"; break;
                case 0x9a: regs.A = ALU8080::sbb(regs.A, regs.D, regs.F); asmInst = "SBB D"; break;
                case 0x9b: regs.A = ALU8080::sbb(regs.A, regs.E, regs.F); asmInst = "SBB E"; break;
                case 0x9c: regs.A = ALU8080::sbb(regs.A, regs.H, regs.F); asmInst = "SBB H"; break;
                case 0x9d: regs.A = ALU8080::sbb(regs.A, regs.L, regs.F); asmInst = "SBB L"; break;
                case 0x9e: regs.A = ALU8080::sbb(regs.A, mem.read(regs.HL()), regs.F); asmInst = "SBB M"; break;
                case 0x9f: regs.A = ALU8080::sbb(regs.A, regs.A, regs.F); asmInst = "SBB A"; break;

                // --- CMP reg ---
                case 0xB8: ALU8080::cmp(regs.A, regs.B, regs.F); asmInst = "CMP B"; break;
                case 0xB9: ALU8080::cmp(regs.A, regs.C, regs.F); asmInst = "CMP C"; break;
                case 0xBA: ALU8080::cmp(regs.A, regs.D, regs.F); asmInst = "CMP D"; break;
                case 0xBB: ALU8080::cmp(regs.A, regs.E, regs.F); asmInst = "CMP E"; break;
                case 0xBC: ALU8080::cmp(regs.A, regs.H, regs.F); asmInst = "CMP H"; break;
                case 0xBD: ALU8080::cmp(regs.A, regs.L, regs.F); asmInst = "CMP L"; break;
                case 0xBE: ALU8080::cmp(regs.A, mem.read(regs.HL()), regs.F); asmInst = "CMP M"; break;
                case 0xBF: ALU8080::cmp(regs.A, regs.A, regs.F); asmInst = "CMP A"; break;

                // --- RNZ ---
                case 0xC0:
                    if(!regs.getFlag(i8080::ZERO)) {
                        regs.PC = pop();
                    }
                    asmInst = "RNZ";
                    break;

                // --- ADI d8 ---
                case 0xC6: {
                        uint8_t data = mem.read(regs.PC++);
                        uint8_t flags = regs.F;
                        regs.A = ALU8080::add(regs.A, data, flags);
                        regs.F = flags;
                        asmInst = "ADI " + Calcs::hex8(data);
                        break;
                    }

                // RST 0
                case 0xC7: push(regs.PC); regs.PC = 0x00; asmInst = "RST 0"; break;
                case 0xCF: push(regs.PC); regs.PC = 0x08; asmInst = "RST 1"; break;
                case 0xD7: push(regs.PC); regs.PC = 0x10; asmInst = "RST 2"; break;
                case 0xDF: push(regs.PC); regs.PC = 0x18; asmInst = "RST 3"; break;
                case 0xE7: push(regs.PC); regs.PC = 0x20; asmInst = "RST 4"; break;
                case 0xEF: push(regs.PC); regs.PC = 0x28; asmInst = "RST 5"; break;
                case 0xF7: push(regs.PC); regs.PC = 0x30; asmInst = "RST 6"; break;
                case 0xFF: push(regs.PC); regs.PC = 0x38; asmInst = "RST 7"; break;

                // --- RZ ---
                case 0xC8: if(regs.getFlag(i8080::ZERO)) regs.PC = pop(); asmInst = "RZ"; break;
                case 0xD0: if(!regs.getFlag(i8080::CARRY)) regs.PC = pop(); asmInst = "RNC"; break;
                case 0xD8: if(regs.getFlag(i8080::CARRY)) regs.PC = pop(); asmInst = "RC"; break;

                // --- ACI d8 ---
                case 0xCE: {
                        uint8_t data = mem.read(regs.PC++);
                        uint8_t flags = regs.F;
                        uint8_t carry = (flags & i8080::CARRY) ? 1: 0;
                        regs.A = ALU8080::add(regs.A, data + carry, flags);
                        regs.F = flags;
                        asmInst = "ACI " + Calcs::hex8(data);
                        break;
                    }

                // --- SUI d8 ---
                case 0xD6: {
                        uint8_t data = mem.read(regs.PC++);
                        uint8_t flags = regs.F;
                        regs.A = ALU8080::sub(regs.A, data, flags);
                        regs.F = flags;
                        asmInst = "SUI " + Calcs::hex8(data);
                        break;
                    }

                case 0xCB: asmInst = "NOP 0xCB"; break;
                case 0xD9: asmInst = "NOP 0xD9"; break;
                case 0xDD: asmInst = "NOP 0xDD"; break;
                case 0xED: asmInst = "NOP 0xED"; break;

                // --- SBI d8 ---
                case 0xDE: {
                        uint8_t data = mem.read(regs.PC++);
                        uint8_t flags = regs.F;
                        uint8_t carry = (flags & i8080::CARRY) ? 1: 0;
                        regs.A = ALU8080::sub(regs.A, data + carry, flags);
                        regs.F = flags;
                        asmInst = "SBI " + Calcs::hex8(data);
                        break;
                    }

                // --- RPO ---
                case 0xE0: if(!regs.getFlag(i8080::PARITY)) regs.PC = pop(); asmInst = "RPO"; break;

                // --- XTHL ---
                case 0xE3: {
                        uint16_t temp = regs.HL();
                        regs.H = mem.read(regs.SP + 1);
                        regs.L = mem.read(regs.SP);
                        mem.write(regs.SP, (temp >> 8) & 0xFF);
                        mem.write(regs.SP, temp & 0xFF);
                        asmInst = "XTHL";
                    }
                    break;

                // --- RPE ---
                case 0xE8: if(regs.getFlag(i8080::PARITY)) regs.PC = pop(); asmInst = "RPE"; break;

                // --- PCHL ---
                case 0xE9: regs.PC = regs.HL(); asmInst = "PCHL"; break;

                // --- XCHG ---
                case 0xEB: {
                        uint8_t tempH = regs.H;
                        uint8_t tempL = regs.L;
                        regs.H = regs.D;
                        regs.L = regs.E;
                        regs.D = tempH;
                        regs.E = tempL;
                        asmInst = "XCHG";
                        break;
                    }

                // --- RP ---
                case 0xF0: if(!regs.getFlag(i8080::SIGN)) regs.PC = pop(); asmInst = "RP"; break;

                // --- JP addr ---
                case 0xF2: {
                        uint16_t addr = Calcs::makeWord(mem.read(regs.PC++), mem.read(regs.PC++));
                        if(!regs.getFlag(i8080::SIGN)) regs.PC = addr;
                        asmInst = "JP [" + Calcs::hex16(addr) + "]";
                        break;
                    }

                // --- DI ---
                case 0xF3: interruptsEnabled = false; asmInst = "DI"; break;

                // --- EI ---
                case 0xFB: interruptsEnabled = true; asmInst = "EI"; break;

                // --- RM ---
                case 0xF8: if(regs.getFlag(i8080::SIGN)) regs.PC = pop(); asmInst = "RM"; break;

                // --- SPHL ---
                case 0xF9: regs.SP = regs.HL(); asmInst = "SPHL"; break;

                case 0xFD: asmInst = "NOP 0xFD"; break;

                default:
                    asmInst = "??? 0x" + Calcs::hex8(opcode);
                    break;
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            lastCycleTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            #ifdef DEBUG_SHOW_CONSOLE_MESSAGES
                bool notifyCycleTooLong = false, notifyCycleTooShort = false, notifyCycleEqual = false;
                int executeTime = lastCycleTime - timeInst;

                if(executeTime > 0) {
                    Helpers::busyWait(std::chrono::microseconds(executeTime));
                    notifyCycleTooLong = true;
                    currentTimer += executeTime;
                } else if(executeTime < 0) {
                    notifyCycleTooShort = true;
                    currentTimer += executeTime;
                } else {
                    notifyCycleEqual = true;
                }

                if(currentTimer < -10) {
                    Helpers::busyWait(std::chrono::microseconds(-currentTimer));
                    totalSpeeding += currentTimer;
                    currentTimer = 0;
                }

                if(lastIntructions.size() >= lastInstructionsStoredCount) lastIntructions.pop_front();
                lastIntructions.push_back(asmInst);
                executedInstructions++;

                std::cout 
                    << "|" << executedInstructions << "| "
                    << "[" 
                    << Calcs::hex16(regs.PC) 
                    << "][ " 
                    << std::hex 
                    << static_cast<int>(opcode) 
                    << " ] " 
                    << asmInst 
                    << " (cycles: " 
                    << std::dec
                    << Timings8080::instructionCycles[opcode] 
                    << ") realtime: " 
                    << lastCycleTime 
                    << "us, expected: " 
                    << timeInst 
                    << "us" 
                    << (notifyCycleTooLong ? " \033[31m(CYCLE TOO LONG!)\033[0m" : "")
                    << (notifyCycleTooShort ? " \033[33m(CYCLE TOO SHORT!)\033[0m" : "")
                    << (notifyCycleEqual ? " \033[32m(CYCLE OK)\033[0m" : "")
                    << " TIMER: " << currentTimer << "us"
                    << std::endl;
            #endif
        }

        void dumpStateToConsole() {
            std::cout << "CPU: (8080)" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << "Registers: " << std::endl;
            std::cout << "A:  " << Calcs::hex8(regs.A) << std::endl;
            std::cout << "B:  " << Calcs::hex8(regs.B) << std::endl;
            std::cout << "C:  " << Calcs::hex8(regs.C) << std::endl;
            std::cout << "D:  " << Calcs::hex8(regs.D) << std::endl;
            std::cout << "E:  " << Calcs::hex8(regs.E) << std::endl;
            std::cout << "H:  " << Calcs::hex8(regs.H) << std::endl;
            std::cout << "L:  " << Calcs::hex8(regs.L) << std::endl;
            std::cout << "PC: " << Calcs::hex16(regs.PC) << std::endl;
            std::cout << "SP: " << Calcs::hex16(regs.SP) << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << "Flags: " << std::endl;
            std::cout << "Z:  " << regs.getFlag(i8080::ZERO) << std::endl;
            std::cout << "S:  " << regs.getFlag(i8080::SIGN) << std::endl;
            std::cout << "P:  " << regs.getFlag(i8080::PARITY) << std::endl;
            std::cout << "CY: " << regs.getFlag(i8080::CARRY) << std::endl;
            std::cout << "AC: " << regs.getFlag(i8080::AUX_CARRY) << std::endl;
        }

        void testOpcodes() {
            #ifdef DEBUG_SHOW_CONSOLE_MESSAGES
                std::cout << "Testing Intel 8080 CPU opcodes..." << std::endl;
                for(uint16_t opcode=0; opcode<=0xFF; opcode++) {
                    regs.reset();
                    running = true;
                    mem.write(0x1000, opcode);
                    mem.write(0x1001, 0x00);
                    mem.write(0x1002, 0x00);
                    regs.PC = 0x1000;

                    std::cout << "TEST> opcode 0x" << std::hex << opcode << " -> ";
                    try {
                        step();
                        std::cout << "OK" << std::endl;
                    } catch(const std::exception& e) {
                        std::cout << "FAILED: " << e.what() << std::endl;
                    }
                }
                std::cout << "Testing opcodes done." << std::endl;
            #endif
        }

        #pragma region --- IRegisters8080 methods ---
        uint8_t A() const override { return regs.A; }
        uint8_t B() const override { return regs.B; }
        uint8_t C() const override { return regs.C; }
        uint8_t D() const override { return regs.D; }
        uint8_t E() const override { return regs.E; }
        uint8_t H() const override { return regs.H; }
        uint8_t L() const override { return regs.L; }

        uint16_t HL() const override { return regs.HL(); }
        uint16_t BC() const override { return regs.BC(); }
        uint16_t DE() const override { return regs.DE(); }

        uint16_t PC() const override { return regs.PC; }
        uint16_t SP() const override { return regs.SP; }

        bool flag(i8080::FLAG flag) const override { return regs.getFlag(flag); }
        #pragma endregion

        #pragma region --- IMemory8080 methods ---
        uint8_t getByte(uint16_t address) const override { return mem.getByte(address); }
        uint16_t getWord(uint16_t address) const override { return mem.getWord(address); }
        std::vector<uint8_t> getBytesBlock(uint16_t address, size_t size) const override { return mem.getBytesBlock(address, size); }
        size_t getSize() const override { return mem.getSize(); }
        bool loadProgram(uint16_t startAddress, std::vector<uint8_t>& buffer) override { return mem.loadProgram(startAddress, buffer); }
        #pragma endregion
};
