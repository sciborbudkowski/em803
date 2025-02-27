// #pragma once

// #include <cstdarg>
// #include <string>
// #include <utility>
// #include <vector>

// #include <raylib.h>

// #include "ITerminalAccess.h"
// #include "Properties.h"

// class TerminalRL : public ITerminalAccess {
//     protected:
//         int widthChars, widthPixels, widthOfCharInPixels;
//         int heightChars, heightPixels, heightOfCharInPixels;
//         int cursorX, cursorY;
//         char lastChar;
//         bool isBusy;
//         float fontSize;
        
//         std::vector<std::string> buffer;
//         std::string lastLine;

//         Font font;

//     public:
//         TerminalRL(int widthChars = TERMINAL_CHARS_WIDTH, int heightChars = TERMINAL_CHARS_HEIGHT, int widthPixels = TERMINAL_PIXELS_WIDTH, int heightPixels = TERMINAL_PIXELS_HEIGHT)
//         : widthChars(widthChars), heightChars(heightChars), widthPixels(widthPixels), heightPixels(heightPixels) {
//             updateScalling();
//             buffer.resize(heightChars, std::string(widthChars, ' '));
//         }

//         #pragma region --- ITerminalAccess methods ---
//         char inputChar() const override { return lastChar; }

//         void outputChar(char ch) override {
//             isBusy = true;
//             if(ch == '\n') {
//                 cursorX = 0;
//                 cursorY++;
//                 if(!buffer.empty()) {
//                     lastLine = buffer.back();
//                 }
//             } else {
//                 buffer[cursorY][cursorX] = ch;
//                 cursorX++;
//                 if(cursorX >= widthChars) {
//                     cursorX = 0;
//                     cursorY++;
//                 }
//             }

//             if(cursorY >= heightChars) {
//                 buffer.erase(buffer.begin());
//                 buffer.push_back(std::string(widthChars, ' '));
//                 cursorY--;
//             }
//             isBusy = false;
//         }

//         void outputString(const char* format, ...) override {
//             isBusy = true;
//             va_list args;
//             va_start(args, format);
//             int size = std::vsnprintf(nullptr, 0, format, args);
//             va_end(args);

//             if(size <= 0) return;

//             std::vector<char> tempBuffer(size);
//             va_start(args, format);
//             std::vsnprintf(tempBuffer.data(), size, format, args);
//             va_end(args);

//             for(char ch : tempBuffer) {
//                 if(ch == '\n') break;
//                 outputChar(ch);
//             }
//             isBusy = false;
//         }

//         void backspace() override {
//             isBusy = true;
//             if(cursorX > 0) {
//                 cursorX--;
//             } else if(cursorY > 0) {
//                 cursorY--;
//                 cursorX = widthChars - 1;
//                 buffer[cursorY][cursorX] = ' ';
//             }
//             isBusy = false;
//         }
        
//         void clear() override {
//             isBusy = true;
//             buffer.clear();
//             buffer.resize(heightChars, std::string(widthChars, ' '));
//             cursorX = 0;
//             cursorY = 0;
//             isBusy = false;
//         }
        
//         std::vector<std::string> getBuffer() const override { return buffer; }

//         std::pair<int, int> getCursorPosition() const override { return std::pair<int, int>(cursorX, cursorY); }

//         void setCursorPosition(std::pair<int, int> position) override {
//             cursorX = position.first;
//             cursorY = position.second;
//         }

//         std::pair<int, int> getCharDimension() const override { return std::pair<int, int>(widthChars, heightChars); }

//         std::pair<int, int> getPixelDimension() const override { return std::pair<int, int>(widthPixels, heightPixels); }

//         void setStatus(bool status) override { isBusy = status; }

//         bool getStatus() const override { return isBusy; }

//         // void render() const override {
//         //     float offsetX = 20.0f, offsetY = 20.0f;

//         //     DrawRectangle(offsetX, offsetY, widthPixels, heightPixels, TERMINAL_BACKGROUND_COLOR);
//         //     for(int y=0; y<heightChars; y++) {
//         //         // DrawTextEx(font, buffer[y].c_str(), Vector2{offsetX, offsetY + y*heightOfCharInPixels}, fontSize, 2, TERMINAL_FOREGROUND_COLOR);
//         //         for(int x=0; x<widthChars; x++) {
//         //             char ch = buffer[y][x];
//         //             if(ch != ' ') {
//         //                 Vector2 pos = {static_cast<float>(x)*widthOfCharInPixels + offsetX, static_cast<float>(y)*heightOfCharInPixels + offsetY};
//         //                 DrawTextEx(font, std::string(1, ch).c_str(), pos, fontSize, 2, TERMINAL_FOREGROUND_COLOR);
//         //             }
//         //         }
//         //     }

//         //     DrawRectangle(offsetX + cursorX*widthOfCharInPixels, offsetY + cursorY*heightOfCharInPixels, widthOfCharInPixels, heightOfCharInPixels, TERMINAL_FOREGROUND_COLOR);
//         // }

//         //void setFont(Font& font) override { this->font = font; }
//         #pragma endregion
    
//     private:
//         void updateScalling() {
//             widthOfCharInPixels = widthPixels / widthChars;
//             heightOfCharInPixels = heightPixels / heightChars;
//             fontSize = static_cast<float>(heightOfCharInPixels);
//         }
// };
