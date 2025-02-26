#pragma once

#include <atomic>
#include <cstdint>
#include <QGroupBox>
#include <QMainWindow>
#include <QMessageBox>
#include <QLayout>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>

#include "CPU8080.h"
#include "CPUWorker.h"
#include "CPUFactory.h"
#include "RegisterViewWidget.h"
#include "Terminal.h"
#include "TextTerminalWidget.h"

class WinEmu8080 : public QMainWindow {
    Q_OBJECT

    private:
        std::shared_ptr<Terminal> terminal;
        std::shared_ptr<CPU8080> cpu;
        CPUWorker* cpuWorker = nullptr;
        QThread* workerThread = nullptr;
        size_t memorySize;
        bool isRunning = false;
        uint16_t memoryTableStartAddress = 0;
        const int memoryTableSize = 0x100;

        void kill(bool withDelete = false) {
            if(cpu) {
                cpu->stop();
            }
            if(cpuWorker) {
                cpuWorker->stop();
            }
            if(workerThread && workerThread->isRunning()) {
                workerThread->disconnect();
                cpuWorker->disconnect();

                workerThread->quit();
                workerThread->wait(1000);

                if(workerThread->isRunning()) {
                    workerThread->terminate();
                    workerThread->wait();
                }

                if(withDelete) {
                    delete cpuWorker;
                    delete workerThread;

                    cpuWorker = nullptr;
                    workerThread = nullptr;
                }
            }
            isRunning = false;
        }

        void prepareUi() {
            QWidget* centralWidget = new QWidget(this);

            QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
            centralWidget->setLayout(mainLayout);

            QHBoxLayout* termAndRegsLayout = new QHBoxLayout();
            mainLayout->addLayout(termAndRegsLayout);
            
            TextTerminalWidget* termWidget = new TextTerminalWidget(terminal, centralWidget);
            termWidget->setMinimumSize(TERMINAL_PIXELS_WIDTH, TERMINAL_PIXELS_HEIGHT);
            termWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            termWidget->setFocus();

            QGroupBox* groupRegisters = new QGroupBox("REGISTERS", centralWidget);
            groupRegisters->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            groupRegisters->setFixedSize(150, 200);
            groupRegisters->setMaximumSize(150, 200);
            termAndRegsLayout->addWidget(termWidget);
            termAndRegsLayout->addWidget(groupRegisters);

            //termAndRegsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
            
            QVBoxLayout* registersLayout = new QVBoxLayout(groupRegisters);

            // Pierwszy rząd
            RegisterViewWidget* regA = new RegisterViewWidget(QString("A:"), groupRegisters);
            registersLayout->addWidget(regA);

            // Drugi rząd
            QHBoxLayout* regsBCLayout = new QHBoxLayout();
            RegisterViewWidget* regB = new RegisterViewWidget(QString("B:"), groupRegisters);
            RegisterViewWidget* regC = new RegisterViewWidget(QString("C:"), groupRegisters);
            regsBCLayout->addWidget(regB);
            regsBCLayout->addWidget(regC);
            registersLayout->addLayout(regsBCLayout);

            // Trzeci rząd
            QHBoxLayout* regsDELayout = new QHBoxLayout();
            RegisterViewWidget* regD = new RegisterViewWidget(QString("D:"), groupRegisters);
            RegisterViewWidget* regE = new RegisterViewWidget(QString("E:"), groupRegisters);
            regsDELayout->addWidget(regD);
            regsDELayout->addWidget(regE);
            registersLayout->addLayout(regsDELayout);

            // Czwarty rząd
            QHBoxLayout* regsHLLayout = new QHBoxLayout();
            RegisterViewWidget* regH = new RegisterViewWidget(QString("H:"), groupRegisters);
            RegisterViewWidget* regL = new RegisterViewWidget(QString("L:"), groupRegisters);
            regsHLLayout->addWidget(regH);
            regsHLLayout->addWidget(regL);
            registersLayout->addLayout(regsHLLayout);

            // Piąty rząd
            QHBoxLayout* regsSPPCLayout = new QHBoxLayout();
            RegisterViewWidget* regSP = new RegisterViewWidget(QString("SP:"), groupRegisters, true);
            RegisterViewWidget* regPC = new RegisterViewWidget(QString("PC:"), groupRegisters, true);
            regsSPPCLayout->addWidget(regSP);
            regsSPPCLayout->addWidget(regPC);
            registersLayout->addLayout(regsSPPCLayout);

            QHBoxLayout* buttonsLayout = new QHBoxLayout();
            mainLayout->addLayout(buttonsLayout);

            QPushButton* sizeButton = new QPushButton("SIZE");
            QPushButton* startButton = new QPushButton("START");
            QPushButton* stopButton = new QPushButton("STOP");
            buttonsLayout->addWidget(sizeButton);
            buttonsLayout->addWidget(startButton);
            buttonsLayout->addWidget(stopButton);

            QGroupBox* groupMemory = new QGroupBox("MEMORY", centralWidget);
            groupMemory->setFixedHeight(250);
            mainLayout->addWidget(groupMemory);

            QTableWidget* memoryTable = new QTableWidget(10, 16, groupMemory);
            memoryTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

            for(int col=0; col<16; ++col) {
                QString header = QString("0x%1").arg(col, 0, 16).toUpper();
                memoryTable->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
            }
            std::vector<uint8_t> buffer = cpu->getBytesBlock(memoryTableStartAddress, memoryTableSize);
            for(int row=0; row<10; ++row) {
                for(int col=0; col<16; ++col) {
                    int index = row*16 + col;
                    if(index < memorySize) {
                        QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(buffer[index], 2, 16, QChar('0').toUpper()));
                        memoryTable->setItem(row, col, item);
                    }
                }
            }

            memoryTable->resizeColumnsToContents();

            QVBoxLayout* memoryLayout = new QVBoxLayout();
            memoryLayout->addWidget(memoryTable);
            groupMemory->setLayout(memoryLayout);

            connect(stopButton, &QPushButton::clicked, this, [this]() { cpu->stop(); });
            connect(startButton, &QPushButton::clicked, this, [this]() { run(); });
            connect(sizeButton, &QPushButton::clicked, this, [this]() {
                std::cout << "Width: " << this->width();
                std::cout << " | Height: " << this->height() << std::endl;
            });

            setCentralWidget(centralWidget);
        }

    public:
        explicit WinEmu8080(QWidget* parent = nullptr, size_t memorySize = 0x10000) : QMainWindow(parent), memorySize(memorySize) {
            try {
                terminal = std::make_shared<Terminal>();
                cpu = std::make_shared<CPU8080>(terminal, memorySize);

                cpuWorker = new CPUWorker(cpu);
                workerThread = new QThread;

                resize(WINDOW_WIDTH, WINDOW_HEIGHT);
                setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

                prepareUi();
            } catch(const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Failed to start emulator: %1").arg(e.what()));
                throw;
            }
        }

        ~WinEmu8080() override { kill(true); }

        void run() {
            if(workerThread && cpuWorker) {
                kill();

                if(!cpuWorker) cpuWorker = new CPUWorker(cpu);
                if(!workerThread) workerThread = new QThread;
                workerThread->disconnect();
                cpuWorker->disconnect();

                cpuWorker->moveToThread(workerThread);
                connect(workerThread, &QThread::started, cpuWorker, &CPUWorker::run);
                connect(cpuWorker, &CPUWorker::stopped, workerThread, &QThread::quit);
                connect(workerThread, &QThread::finished, this, [this]() {
                    std::cout << "WinEmu8080::run() - Worker thread finished." << std::endl;
                    isRunning = false;
                });

                workerThread->start();
                isRunning = true;
                show();
            }
        }

    protected:
        void closeEvent(QCloseEvent* event) override {
            kill();
            QMainWindow::closeEvent(event);
        }
};
