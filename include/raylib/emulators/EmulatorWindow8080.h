#pragma once

#include "RaylibWrapper.h"
#include "raygui.h"
#include "Properties.h"
#include "CPU8080.h"
#include "Memory8080.h"
#include "IOPorts8080.h"
#include "ITerminalAccess.h"
#include "StartAddress8080Dialog.h"
#include "UIRegiterBox.h"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>

class EmulatorWindow8080 {
    private:
        CPU8080 cpu;
        ITerminalAccess& terminal;

        size_t memorySize;

        int winWidth, panelWidth, pX;
        bool showingMemoryView;
        const int termX = 20 + 80*9 + 20;
        const float buttonsY = 16*25 + 2*20;
        unsigned int memoryViewOffset = 0x0000;

        void init() {
            cpu.reset();
            cpu.stop();
        }

        void handleEvents() {
            switch(GetKeyPressed()) {
                case KEY_BACKSPACE: terminal.backspace(); break;
                case KEY_ENTER: terminal.outputChar('\n'); break;
                case KEY_SPACE: terminal.outputChar(' '); break;
                default: {
                    std::string ch = ""; // must check what is this for?!?!
                    for(int key=KEY_A; key<=KEY_Z; key++) {
                        if(IsKeyPressed(key)) {
                            char chKey = key - KEY_A + 'a';
                            terminal.outputChar(chKey);
                            ch = chKey;
                        }
                    }
                    for(int key=KEY_ZERO; key<=KEY_NINE; key++) {
                        if(IsKeyPressed(key)) {
                            char chKey = key - KEY_ZERO + '0';
                            terminal.outputChar(chKey);
                            ch = chKey;
                        }
                    }
                    break;
                }
            }
        }

        void recalcUI() {
            winWidth = GetScreenWidth();
            panelWidth = winWidth - termX;
            pX = termX + panelWidth - 80;
        }

        void renderRegistersView() {
            GuiGroupBox(Rectangle{static_cast<float>(termX) - 10, 20, static_cast<float>(panelWidth), 135}, "REGISTERS");
            UIRegisterBox::renderRegistersPanel(termX, 30.0f, "A:", cpu.A());
            UIRegisterBox::renderRegistersPanel(termX, 55.0f, "B:", cpu.B());
            UIRegisterBox::renderRegistersPanel(pX + 5, 55.0f, "C:", cpu.C());
            UIRegisterBox::renderRegistersPanel(termX, 80.0f, "D:", cpu.D());
            UIRegisterBox::renderRegistersPanel(pX + 5, 80.0f, "E:", cpu.E());
            UIRegisterBox::renderRegistersPanel(termX, 105.0f, "H:", cpu.H());
            UIRegisterBox::renderRegistersPanel(pX + 5, 105.0f, "L:", cpu.L());
            UIRegisterBox::renderRegistersPanel(termX - 5, 130.0f, "PC:", cpu.PC());
            UIRegisterBox::renderRegistersPanel(pX, 130.0f, "SP:", cpu.SP());
        }

        void renderFlagsView() {
            GuiGroupBox(Rectangle{static_cast<float>(termX) - 10, 165, static_cast<float>(panelWidth), 60}, "FLAGS");
            UIRegisterBox::renderFlagsPanel(termX, 175, "Z:", cpu.flag(i8080::ZERO));
            UIRegisterBox::renderFlagsPanel(termX + 45, 175, "S:", cpu.flag(i8080::SIGN));
            UIRegisterBox::renderFlagsPanel(termX + 90, 175, "P:", cpu.flag(i8080::PARITY));
            UIRegisterBox::renderFlagsPanel(termX - 5, 200, "C:", cpu.flag(i8080::CARRY));
            UIRegisterBox::renderFlagsPanel(termX + 40, 200, "AC:", cpu.flag(i8080::AUX_CARRY));
        }

        void renderAssemblerView() {
            float castedTermX = static_cast<float>(termX);
            float castedPanelWidth = static_cast<float>(panelWidth);

            GuiGroupBox(Rectangle{castedTermX - 10, 235, castedPanelWidth, 100}, "ASM");
            for(float i=4; i>=0; i--) {
                if(i == 4) {
                    DrawRectangleRec(Rectangle{castedTermX, 245 + i*15 + 5, castedPanelWidth - 20, 10}, DARKGREEN);
                }
                std::string label = " ";
                if(terminal.getBuffer().size() > i) label = terminal.getBuffer()[i];

                GuiLabel(Rectangle{castedTermX + 15, 245 + i*15, castedPanelWidth - 30, 20}, label.c_str());
            }
        }

        void renderAndHandleButtons() {
            if(GuiButton(Rectangle{20, buttonsY, 100, 30}, cpu.isRunning() ? "Stop" : "Start")) {
                cpu.isRunning() ? cpu.stop() : cpu.run();
            }
            if(GuiButton(Rectangle{130, buttonsY, 100, 30}, "Load PRG")) {
                uint16_t startAddressForLoadedProgram = 0x100;

                QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Program", ".", "Binary files (*.bin)");
                if(!fileName.isEmpty()) {
                    std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary);
                    if(!file) {
                        QMessageBox::critical(nullptr, "Load Error", QString("Can not load program from file %1").arg(QString::fromStdString(fileName.toStdString())));
                        return;
                    }

                    StartAddress8080Dialog dialog(nullptr, 0x0100);
                    if(dialog.exec() == QDialog::Accepted)
                        startAddressForLoadedProgram = dialog.getStartAddress();

                    file.seekg(0, std::ios::end);
                    size_t fileSize = file.tellg();
                    file.seekg(0, std::ios::beg);

                    if(fileSize > cpu.getSize() - 0x0100) {
                        QMessageBox::critical(nullptr, "Load Error", QString("Program is too large to fit in memory"));
                        return;
                    }

                    std::vector<uint8_t> buffer(fileSize);
                    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
                    if(cpu.loadProgram(startAddressForLoadedProgram, buffer)) {
                        terminal.outputString("Loaded program from %s to address: %04Xh", fileName.toStdString().c_str(), startAddressForLoadedProgram);
                        terminal.outputChar('\n');
                    } else {
                        QMessageBox::critical(nullptr, "Load Error", QString("Can not load program from file %1").arg(QString::fromStdString(fileName.toStdString())));
                    }
                }
            }
            if(GuiButton(Rectangle{240, buttonsY, 100, 30}, "Load DSK")) {
                QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Disk", ".", "Disk files (*.dsk)");
                if(!fileName.isEmpty()) {
                    //cpu.disk.loadDiskFromFile(fileName.toStdString(), cpu.memory, 0x0100);
                    //cpu->get
                    //terminal->printString("Loaded disk from %s\n", fileName.toStdString().c_str());
                    std::cout << "Loading binary file not implemented yet." << std::endl;
                }
            }
            if(GuiButton(Rectangle{350, buttonsY, 100, 30}, "Step >")) {
                cpu.step();
            }
            if(GuiButton(Rectangle{460, buttonsY, 100, 30}, "Reset")) {
                cpu.reset();
            }
            if(GuiButton(Rectangle{570, buttonsY, 100, 30}, "Dump CPU")) {
                cpu.dumpStateToConsole();
            }
            if(GuiButton(Rectangle{680, buttonsY, 100, 30}, "Dump MEM")) {
                showingMemoryView = !showingMemoryView;
                if(showingMemoryView) {
                    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT + 350);
                    memoryViewOffset = 0x0100;
                } else {
                    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                }
            }
            if(GuiButton(Rectangle{790, buttonsY, 100, 30}, "Test Opcodes")) {
                std::cout << "EmulatorWindow::start() Test Opcodes" << std::endl;
                cpu.testOpcodes();
            }
        }

    public:
        EmulatorWindow8080(ITerminalAccess& terminal, size_t memorySize)
        : terminal(terminal), memorySize(memorySize), cpu(terminal, memorySize) {}
        ~EmulatorWindow8080() = default;

        void run() {
            InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Emulator | Intel 8080");
            SetTargetFPS(TARGET_FPS);
            setColors();

            while(!WindowShouldClose()) {
                BeginDrawing();

                ClearBackground(BLACK);
                recalcUI();
                handleEvents();
                terminal.render();
                renderRegistersView();
                renderFlagsView();
                renderAssemblerView();
                renderAndHandleButtons();

                if(cpu.isRunning()) cpu.step();

                EndDrawing();
            }

            CloseWindow();
        }

        void setColors() const {
            GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
            GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
            GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));
        }
};
