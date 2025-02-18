#pragma once

#include <string>
#include <vector>

class ITerminalAccess {
    public:
        virtual ~ITerminalAccess() = default;

        virtual char inputChar() const = 0;
        virtual void outputChar(char ch) = 0;
        virtual void outputString(const char* format, ...) const = 0;
        virtual void backspace() = 0;
        virtual void clear() = 0;

        virtual std::vector<std::string> getBuffer() const = 0;
        virtual std::pair<int, int> getCursorPosition() const = 0;
        virtual void setCursorPosition(std::pair<int, int>) const = 0;

        virtual std::pair<int, int> getCharDimension() const = 0;
        virtual std::pair<int, int> getPixelDimension() const = 0;

        virtual void setStatus(bool status) const = 0;
        virtual bool getStatus() const = 0;
};
