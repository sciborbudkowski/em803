#pragma once

#include <cstdarg>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "ITerminalAccess.h"
#include "RaylibWrapper.h"

#ifndef TERMINAL_CHARS_WIDTH
#define TERMINAL_CHARS_WIDTH 80
#endif
#ifndef TERMINAL_CHARS_HEIGHT
#define TERMINAL_CHARS_HEIGHT 25
#endif
#ifndef TERMINAL_PIXELS_WIDTH
#define TERMINAL_PIXELS_WIDTH 800
#endif
#ifndef TERMINAL_PIXELS_HEIGHT
#define TERMINAL_PIXELS_HEIGHT 600
#endif

class Terminal : public ITerminalAccess {
    protected:
        int widthChars, widthPixels, widthOfCharInPixels;
        int heightChars, heightPixels, heightOfCharInPixels;
        int cursorX, cursorY;
        char lastChar;
        bool isBusy;
        float fontSize;
        
        std::vector<std::string> buffer;
        std::string lastLine;

        Font font;

    public:
        Terminal(Font& font, int widthChars = TERMINAL_CHARS_WIDTH, int heightChars = TERMINAL_CHARS_HEIGHT, int widthPixels = TERMINAL_PIXELS_WIDTH, int heightPixels = TERMINAL_PIXELS_HEIGHT)
        : font(font), widthChars(widthChars), heightChars(heightChars), widthPixels(widthPixels), heightPixels(heightPixels) {
            updateScalling();
            buffer.resize(heightChars, std::string(widthChars, ' '));
        }

        #pragma region ITerminalAccess

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

        void outputString(const char* format, ...) const override {
            
        }

        #pragma endregion

    private:
        void updateScalling() {
            widthOfCharInPixels = widthPixels / widthChars;
            heightOfCharInPixels = heightPixels / heightChars;
            fontSize = static_cast<float>(heightOfCharInPixels);
        }
};
