#pragma once

#include "ITerminalAccess.h"
#include "Properties.h"

#include <QtOpenGLWidgets/qopenglwidget.h>
#include <qfont.h>
#include <qopenglfunctions.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <string>
#include <vector>

class Terminal : public ITerminalAccess {
    Q_OBJECT

    private:
        int widthChars, heightChars;
        int widthPixels, heightPixels;
        int cursorX, cursorY;
        bool isBusy;
        char lastChar;

        std::vector<std::string> buffer;
        std::string lastLine;

    public:
        explicit Terminal(int widthChars = TERMINAL_CHARS_WIDTH,
                          int heightChars = TERMINAL_CHARS_HEIGHT,
                          int widthPixels = TERMINAL_PIXELS_WIDTH,
                          int heightPixels = TERMINAL_PIXELS_HEIGHT
                         )
        : widthChars(widthChars), heightChars(heightChars),
          widthPixels(widthPixels), heightPixels(heightPixels),
          isBusy(false) {
            cursorX = cursorY = 0;
            lastChar = '\0';

            buffer.resize(heightChars, std::string(widthChars, ' '));
          }

        char inputChar() const override { return lastChar; }

        void outputChar(char ch) override {
            isBusy = true;
            if(ch == '\n') {
                cursorX = 0;
                cursorY++;
                if(!buffer.empty()) {
                    lastLine = buffer.back();
                }
            } else {
                buffer[cursorY][cursorX] = ch;
                cursorX++;
                if(cursorX >= widthChars) {
                    cursorX = 0;
                    cursorY++;
                }
            }

            if(cursorY >= heightChars) {
                buffer.erase(buffer.begin());
                buffer.push_back(std::string(widthChars, ' '));
                cursorY--;
            }
            isBusy = false;
        }

        void outputString(const char* format, ...) override {
            isBusy = true;
            va_list args;
            va_start(args, format);
            int size = std::vsnprintf(nullptr, 0, format, args);
            va_end(args);

            if(size <= 0) return;

            std::vector<char> tempBuffer(size);
            va_start(args, format);
            std::vsnprintf(tempBuffer.data(), size, format, args);
            va_end(args);

            for(char ch : tempBuffer) {
                if(ch == '\n') break;
                outputChar(ch);
            }
            isBusy = false;
        }

        void backspace() override {
            isBusy = true;
            if(cursorX > 0) {
                cursorX--;
            } else if(cursorY > 0) {
                cursorY--;
                cursorX = widthChars - 1;
                buffer[cursorY][cursorX] = ' ';
            }
            isBusy = false;
        }

        std::vector<std::string> getBuffer() const override { return buffer; }

        std::pair<int, int> getCursorPosition() const override { return std::pair<int, int>(cursorX, cursorY); }

        void setCursorPosition(std::pair<int, int> position) override {
            cursorX = position.first;
            cursorY = position.second;
        }

        std::pair<int, int> getCharDimension() const override { return std::pair<int, int>(widthChars, heightChars); }

        std::pair<int, int> getPixelDimension() const override { return std::pair<int, int>(widthPixels, heightPixels); }

        void setStatus(bool status) override { isBusy = status; }

        bool getStatus() const override { return isBusy; }

        void setLastChar(char ch) override { lastChar = ch; }
};
