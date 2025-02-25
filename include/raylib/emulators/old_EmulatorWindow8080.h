// #pragma once

// #include "AssetLoader.h"
// #include "raygui.h"
// #include "Properties.h"
// #include "CPU8080.h"
// #include "ITerminalAccess.h"
// #include "StartAddress8080Dialog.h"
// #include "UIRegiterBox.h"

// #include <cstdint>
// #include <iostream>
// #include <fstream>
// #include <raylib.h>
// #include <unistd.h>
// #include <QMessageBox>
// #include <QFileDialog>
// #include <QString>

// class EmulatorWindow8080 {
//     private:
//         CPU8080 cpu;
//         ITerminalAccess& terminal;

//         size_t memorySize;

//         float winWidth, sidePanelsWidth, sidePanelsX;
//         bool showingMemoryView;
//         const float buttonsY = 16*25 + 2*20;
//         unsigned int memoryViewOffset = 0x0000;

//         Font font;

//         void init() {
//             cpu.reset();
//             cpu.stop();
//         }

//         void handleEvents() {
//             switch(GetKeyPressed()) {
//                 case KEY_BACKSPACE: terminal.backspace(); break;
//                 case KEY_ENTER: terminal.outputChar('\n'); break;
//                 case KEY_SPACE: terminal.outputChar(' '); break;
//                 default: {
//                     std::string ch = ""; // must check what is this for?!?!
//                     for(int key=KEY_A; key<=KEY_Z; key++) {
//                         if(IsKeyPressed(key)) {
//                             char chKey = key - KEY_A + 'a';
//                             terminal.outputChar(chKey);
//                             ch = chKey;
//                         }
//                     }
//                     for(int key=KEY_ZERO; key<=KEY_NINE; key++) {
//                         if(IsKeyPressed(key)) {
//                             char chKey = key - KEY_ZERO + '0';
//                             terminal.outputChar(chKey);
//                             ch = chKey;
//                         }
//                     }
//                     break;
//                 }
//             }
//         }

//         void recalcUI() {
//             winWidth = GetScreenWidth();
//             sidePanelsX = TERMINAL_PIXELS_WIDTH + 20 + 10;
//             sidePanelsWidth = winWidth - sidePanelsX - 10;
//         }

//         void renderRegistersView() {
//             float startY = 20.0f;
//             float height = 135.0f;
//             float leftFieldX = sidePanelsX + 10;
//             float rightFieldX = sidePanelsX + sidePanelsWidth/2 + 10;
//             std::cout << "registers: left" << leftFieldX << ", right " << rightFieldX << std::endl;

//             GuiGroupBox(Rectangle{sidePanelsX, startY, sidePanelsWidth, height}, "REGISTERS");
//             UIRegisterBox::renderRegistersPanel(leftFieldX, 30.0f, "A:", cpu.A());
//             UIRegisterBox::renderRegistersPanel(leftFieldX, 55.0f, "B:", cpu.B());
//             UIRegisterBox::renderRegistersPanel(rightFieldX, 55.0f, "C:", cpu.C());
//             UIRegisterBox::renderRegistersPanel(leftFieldX, 80.0f, "D:", cpu.D());
//             UIRegisterBox::renderRegistersPanel(rightFieldX, 80.0f, "E:", cpu.E());
//             UIRegisterBox::renderRegistersPanel(leftFieldX, 105.0f, "H:", cpu.H());
//             UIRegisterBox::renderRegistersPanel(rightFieldX, 105.0f, "L:", cpu.L());
//             UIRegisterBox::renderRegistersPanel(leftFieldX - 5, 130.0f, "PC:", cpu.PC());
//             UIRegisterBox::renderRegistersPanel(rightFieldX - 5, 130.0f, "SP:", cpu.SP());
//         }

//         void renderFlagsView() {
//             float startY = 165.0f;
//             float height = 60.0f;
//             float leftFieldX = sidePanelsX + 10;
//             std::cout << "flags left" << leftFieldX << std::endl;

//             GuiGroupBox(Rectangle{sidePanelsX, startY, sidePanelsWidth, height}, "FLAGS");
//             UIRegisterBox::renderFlagsPanel(leftFieldX, 175, "Z:", cpu.flag(i8080::ZERO));
//             UIRegisterBox::renderFlagsPanel(leftFieldX + 45, 175, "S:", cpu.flag(i8080::SIGN));
//             UIRegisterBox::renderFlagsPanel(leftFieldX + 85, 175, "P:", cpu.flag(i8080::PARITY));
//             UIRegisterBox::renderFlagsPanel(leftFieldX, 200, "C:", cpu.flag(i8080::CARRY));
//             UIRegisterBox::renderFlagsPanel(leftFieldX + 45, 200, "AC:", cpu.flag(i8080::AUX_CARRY));
//         }

//         void renderAssemblerView() {

//             GuiGroupBox(Rectangle{sidePanelsX, 235, sidePanelsWidth, 100}, "ASM");
//             for(float i=4; i>=0; i--) {
//                 if(i == 4) {
//                     DrawRectangleRec(Rectangle{sidePanelsX, 245 + i*15 + 5, sidePanelsWidth - 20, 10}, DARKGREEN);
//                 }
//                 std::string label = " ";
//                 if(terminal.getBuffer().size() > i) label = terminal.getBuffer()[i];

//                 GuiLabel(Rectangle{sidePanelsX + 15, 245 + i*15, sidePanelsWidth - 30, 20}, label.c_str());
//             }
//         }

//         void renderAndHandleButtons() {
//             float width = 75;
//             float x = 20;
//             float space = 10;

//             if(GuiButton(Rectangle{20, buttonsY, width, 30}, cpu.isRunning() ? "Stop" : "Start")) {
//                 cpu.isRunning() ? cpu.stop() : cpu.run();
//             }
//             if(GuiButton(Rectangle{x + width + space, buttonsY, width, 30}, "Load PRG")) {
//                 uint16_t startAddressForLoadedProgram = 0x100;

//                 QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Program", ".", "Binary files (*.bin)");
//                 if(!fileName.isEmpty()) {
//                     std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary);
//                     if(!file) {
//                         QMessageBox::critical(nullptr, "Load Error", QString("Can not load program from file %1").arg(QString::fromStdString(fileName.toStdString())));
//                         return;
//                     }

//                     StartAddress8080Dialog dialog(nullptr, 0x0100);
//                     if(dialog.exec() == QDialog::Accepted)
//                         startAddressForLoadedProgram = dialog.getStartAddress();

//                     file.seekg(0, std::ios::end);
//                     size_t fileSize = file.tellg();
//                     file.seekg(0, std::ios::beg);

//                     if(fileSize > cpu.getSize() - 0x0100) {
//                         QMessageBox::critical(nullptr, "Load Error", QString("Program is too large to fit in memory"));
//                         return;
//                     }

//                     std::vector<uint8_t> buffer(fileSize);
//                     file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
//                     if(cpu.loadProgram(startAddressForLoadedProgram, buffer)) {
//                         terminal.outputString("Loaded program from %s to address: %04Xh", fileName.toStdString().c_str(), startAddressForLoadedProgram);
//                         terminal.outputChar('\n');
//                     } else {
//                         QMessageBox::critical(nullptr, "Load Error", QString("Can not load program from file %1").arg(QString::fromStdString(fileName.toStdString())));
//                     }
//                 }
//             }
//             if(GuiButton(Rectangle{x + width*2 + space*2, buttonsY, width, 30}, "Load DSK")) {
//                 QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Disk", ".", "Disk files (*.dsk)");
//                 if(!fileName.isEmpty()) {
//                     //cpu.disk.loadDiskFromFile(fileName.toStdString(), cpu.memory, 0x0100);
//                     //cpu->get
//                     //terminal->printString("Loaded disk from %s\n", fileName.toStdString().c_str());
//                     std::cout << "Loading binary file not implemented yet." << std::endl;
//                 }
//             }
//             if(GuiButton(Rectangle{x + width*3 + space*3, buttonsY, width, 30}, "Step >")) {
//                 cpu.step();
//             }
//             if(GuiButton(Rectangle{x + width*4 + space*4, buttonsY, width, 30}, "Reset")) {
//                 cpu.reset();
//             }
//             if(GuiButton(Rectangle{x + width*5 + space*5, buttonsY, width, 30}, "Dump CPU")) {
//                 cpu.dumpStateToConsole();
//             }
//             if(GuiButton(Rectangle{x + width*6 + space*6, buttonsY, width, 30}, "Dump MEM")) {
//                 showingMemoryView = !showingMemoryView;
//                 if(showingMemoryView) {
//                     SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT + 350);
//                     memoryViewOffset = 0x0100;
//                 } else {
//                     SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
//                 }
//             }
//             if(GuiButton(Rectangle{x + width*7 + space*7, buttonsY, width, 30}, "Test Opcodes")) {
//                 std::cout << "EmulatorWindow::start() Test Opcodes" << std::endl;
//                 cpu.testOpcodes();
//             }
//         }

//         void renderMemoryView() {
//             float posY = buttonsY + 40;
//             GuiGroupBox(Rectangle{20, posY, static_cast<float>(winWidth) - 40, 350}, "Memory Dump");

//             // every single byte
//             for(int i=0; i<=0xF; i++) {
//                 char colLabel[4];
//                 snprintf(colLabel, sizeof(colLabel), "%02Xh", i);
//                 float posX = 100 + i*50;
//                 DrawTextEx(font, colLabel, Vector2{posX, posY + 10}, 18, 1, WHITE);
//             }

//             // every 16 bytes
//             for(int i=0; i<=0xF; i++) {
//                 char rowLabel[6];
//                 snprintf(rowLabel, sizeof(rowLabel), "%04Xh", memoryViewOffset + i*16);
//                 float posX = 30;
//                 float posY = buttonsY + 80 + i*20;
//                 DrawTextEx(font, rowLabel, Vector2{posX, posY}, 18, 1, WHITE);
//             }

//             // memory dump
//             for(int i=0; i<=0xF; i++) {
//                 for(int j=0; j<=0xF; j++) {
//                     uint16_t pc = memoryViewOffset + i*16 + j;
//                     if(pc == cpu.PC()) {
//                         DrawRectangle(100 + j*50, buttonsY + 80 + i*20, 30, 15, RED);
//                     }
//                     auto color = WHITE;
//                     if(j % 2 == 0) color = LIGHTGRAY;
//                     char memLabel[4];
//                     snprintf(memLabel, sizeof(memLabel), "%02Xh", cpu.getByte(pc));
//                     float posX = 100 + j*50;
//                     float posY = buttonsY + 80 + i*20;
//                     DrawTextEx(font, memLabel, Vector2{posX, posY}, 16, 1, color);
//                 }
//             }
//         }

//     public:
//         EmulatorWindow8080(ITerminalAccess& terminal, size_t memorySize)
//         : terminal(terminal), memorySize(memorySize), cpu(terminal, memorySize) {}
//         ~EmulatorWindow8080() = default;

//         void run() {
//             try {
//                 font = Helpers::AssetLoader::loadFontFile("assets/fonts/DejaVuSansMono.ttf");
//                 //terminal.setFont(font);
//             }
//             catch(const std::exception& e) {
//                 QMessageBox::critical(nullptr, "Load Font Error", QString("Can not load the terminal font.\n\r") + QString(e.what()));
//                 return;
//             }

//             InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Emulator | Intel 8080");
//             SetTargetFPS(TARGET_FPS);
//             setColors();

//             while(!WindowShouldClose()) {
//                 BeginDrawing();

//                 ClearBackground(BLACK);
//                 recalcUI();
//                 handleEvents();
//                 //terminal.render();
//                 renderRegistersView();
//                 renderFlagsView();
//                 renderAssemblerView();
//                 renderAndHandleButtons();
//                 if(showingMemoryView) renderMemoryView();

//                 if(cpu.isRunning()) cpu.step();

//                 EndDrawing();
//             }

//             CloseWindow();
//         }

//         void setColors() const {
//             GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
//             GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
//             GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
//             GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));
//         }
// };
