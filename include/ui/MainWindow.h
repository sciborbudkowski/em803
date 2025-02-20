#pragma once

#include "MachineProfile.h"
#include "MachinesManager.h"
#include "AddMachineDialog.h"
#include "AssetLoader.h"
#include "EmulatorWindow8080.h"
#include "Terminal.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QVector>

#include <iostream>

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr) {
            QWidget* centralWidget = new QWidget(this);
            setCentralWidget(centralWidget);

            QVBoxLayout* layout = new QVBoxLayout(centralWidget);

            machinesList = new QListWidget(this);
            layout->addWidget(machinesList);

            createButton = new QPushButton("New...", this);
            startButton = new QPushButton("Run", this);
            startButton->setEnabled(false);
            editButton = new QPushButton("Edit...", this);
            editButton->setEnabled(false);
            deleteButton = new QPushButton("Delete...", this);
            deleteButton->setEnabled(false);
            exitAppButton = new QPushButton("Quit", this);

            layout->addWidget(createButton);
            layout->addWidget(startButton);
            layout->addWidget(editButton);
            layout->addWidget(deleteButton);
            layout->addWidget(exitAppButton);

            connect(createButton, &QPushButton::clicked, this, &MainWindow::createMachine);
            connect(startButton, &QPushButton::clicked, this, &MainWindow::startMachine);
            connect(machinesList, &QListWidget::itemSelectionChanged, this, &MainWindow::updateButtons);
            connect(deleteButton, &QPushButton::clicked, this, &MainWindow::removeMachine);
            connect(exitAppButton, &QPushButton::clicked, this, &MainWindow::close);

            loadMachines();
            loadAssets();
        }

        ~MainWindow() = default;

        CPUType getSelectedCPUType(int index);

    private:
        QPushButton* createButton;
        QPushButton* startButton;
        QPushButton* editButton;
        QPushButton* deleteButton;
        QPushButton* exitAppButton;
        QListWidget* machinesList;
        MachinesManager machinesManager;
        
        QVector<MachineProfile> machines;

        Font font;

    private slots:
        void createMachine() {
            AddMachineDialog dialog(this);
            if(dialog.exec() == QDialog::Accepted) {
                MachineProfile machine = dialog.getMachine();
                machinesManager.addMachine(machine);
                if(machinesManager.saveMachines()) {
                    loadMachines();
                } else {
                    QMessageBox::warning(this, "Error", "Failed to save machines.");
                }
            }
        }

        void startMachine() {
            QListWidgetItem* item = machinesList->currentItem();
            if (!item) return;

            int machineIndex = item->data(Qt::UserRole).toInt();
            if (machineIndex >= 0 && machineIndex < machines.size()) {
                MachineProfile selectedMachine = machines[machineIndex];
                std::cout << "Starting machine: " << selectedMachine.getMachineName().toStdString() << ", CPU Type: " << MachineProfile::getCPUName(selectedMachine.getProcessorType()).toStdString() 
                    << ", Memory: " << selectedMachine.getMemorySize() << std::endl;

                this->hide();

                switch(selectedMachine.getProcessorType()) {
                    case CPUType::I8080: {
                        std::cout << "Creating emulator" << std::endl;
                        Terminal term = Terminal(font);
                        EmulatorWindow8080 emulator = EmulatorWindow8080(term, 0x10000);
                        
                        std::cout << "Show emulator window" << std::endl;
                        emulator.run();
                    }
                    break;
                    
                    default:
                        std::cout << "Starting of " << MachineProfile::getCPUName(selectedMachine.getProcessorType()).toStdString() << " not implemented yet!" << std::endl;
                        break;
                }

                this->show();
            } else {
                QMessageBox::warning(this, "Error", "Invalid machine selection.");
            }
        }

        void updateButtons() {
            auto status = !machinesList->selectedItems().isEmpty();

            startButton->setEnabled(status);
            editButton->setEnabled(status);
            deleteButton->setEnabled(status);
        }

        void removeMachine() {
            QListWidgetItem* item = machinesList->currentItem();
            QString machineName = item->text();

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Confirm", "Are you sure you want to delete this machine?", QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::Yes) {
                machinesManager.removeMachine(machineName);

                if(machinesManager.saveMachines()) {
                    loadMachines();
                } else {
                    QMessageBox::warning(this, "Error", "Failed to save machines.");
                }
            }
        }

        void loadMachines() {
            machinesList->clear();
            machines = machinesManager.getMachines();
            for(int i = 0; i < machines.size(); ++i) {
                const auto& machine = machines[i];
                QListWidgetItem* item = new QListWidgetItem(machine.getMachineName());
                item->setData(Qt::UserRole, i);
                machinesList->addItem(item);
            }
        }

        void loadAssets() {
            try {
                font = Helpers::AssetLoader::loadFont();
            } catch(const std::runtime_error& e) {
                QMessageBox::critical(nullptr, "Error", e.what());
                return;
            }
        }
};
