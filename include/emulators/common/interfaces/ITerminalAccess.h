#pragma once

#include <QObject>

#include <string>
#include <vector>
#include <utility>

class ITerminalAccess : public QObject {
    Q_OBJECT
    
    public:
        virtual ~ITerminalAccess() = default;

        virtual char inputChar() const = 0;
        virtual void outputChar(char ch) = 0;

        virtual void outputString(const char* format, ...) {}
        virtual void backspace() {}
        virtual void clear() {}
        virtual std::vector<std::string> getBuffer() const { return {}; }
        virtual std::pair<int, int> getCursorPosition() const { return {0, 0}; }
        virtual void setCursorPosition(std::pair<int, int> position) {}
        virtual std::pair<int, int> getCharDimension() const { return {0, 0}; }
        virtual std::pair<int, int> getPixelDimension() const { return {0, 0}; }
        virtual void setStatus(bool status) {}
        virtual bool getStatus() const { return false; }
        virtual void setLastChar(char ch) {}
};
