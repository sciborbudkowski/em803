#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include "ITerminalAccess.h"

class IOPorts8080 {
    protected:
        std::array<uint8_t, 0x100> ports;
        std::shared_ptr<ITerminalAccess> terminal;

    public:
        IOPorts8080(std::shared_ptr<ITerminalAccess> terminal) : terminal(terminal) { reset(); }
        ~IOPorts8080() = default;

        void reset() {
            ports.fill(0);
        }

        void out(uint8_t port, uint8_t value) {
            switch(port) {
                case 0x01: /* output character to termminal */
                    if (terminal) {
                        terminal->outputChar(static_cast<char>(value));
                    }
                    break;
                case 0x11: /* disk selection */
                    ports[port] = value;
                    break;
                case 0x12: /* sector selection */
                    ports[port] = value;
                    break;
                default:
                    ports[port] = value;
            }
        }

        void outString(const char* outString) {
            std::string_view stringView = outString;
            for(char ch : stringView) {
                out(0x01, ch);
            }
        }

        uint8_t in(uint8_t port) {
            switch(port) {
                case 0x02: /* input character from stdin */
                    if (terminal) {
                        return terminal->inputChar();
                    }
                    return 0;
                case 0x10: /* check if disk is ready */
                    return ports[port];
                case 0x13: /* read from disk */
                    return ports[port];
                default:
                    return ports[port];
            }
        }
};
