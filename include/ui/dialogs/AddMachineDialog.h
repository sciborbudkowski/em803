#pragma once

#include "MachineProfile.h"

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>

class AddMachineDialog : public QDialog {
    Q_OBJECT

    public:
        explicit AddMachineDialog(QWidget* parent = nullptr) {
            setWindowTitle("Add New Machine");

            auto* layout = new QVBoxLayout(this);

            auto* processorLabel = new QLabel("Processor type:", this);
            processorCombo = new QComboBox(this);
            for(int i=0; i<=static_cast<int>(CPUType::I8086); i++) {
                auto type = static_cast<CPUType>(i);
                processorCombo->addItem(QString::fromStdString(MachineProfile::getCPUName(type).toStdString()), QVariant(i));
            }

            auto* nameLabel = new QLabel("Machine name:", this);
            nameEdit = new QLineEdit("New machine", this);

            auto* memoryLabel = new QLabel("Memory size:", this);
            memorySizeCombo = new QComboBox(this);

            auto* buttonsLayout = new QHBoxLayout();
            okButton = new QPushButton("OK", this);
            cancelButton = new QPushButton("Cancel", this);
            buttonsLayout->addWidget(okButton);
            buttonsLayout->addWidget(cancelButton);

            layout->addWidget(processorLabel);
            layout->addWidget(processorCombo);
            layout->addWidget(nameLabel);
            layout->addWidget(nameEdit);
            layout->addWidget(memoryLabel);
            layout->addWidget(memorySizeCombo);
            layout->addLayout(buttonsLayout);

            connect(okButton, &QPushButton::clicked, this, &AddMachineDialog::onOkButtonClicked);
            connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
            connect(processorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddMachineDialog::onProcessorTypeChanged);
            onProcessorTypeChanged(0);
        }

        ~AddMachineDialog() = default;

        MachineProfile getMachine() const { return profile; }

    private slots:
        void onOkButtonClicked() {
            QString name = nameEdit->text().trimmed();
            QString processor = processorCombo->currentText();

            if(name.isEmpty() || processor.isEmpty()) {
                QMessageBox::warning(this, "Error", "Machine name can not be empty.");
                return;
            }
            if(processorCombo->currentData().toInt() == static_cast<int>(CPUType::NOT_SET)) {
                QMessageBox::warning(this, "Error", "No processor type selected.");
                return;
            }

            CPUType cpuType = static_cast<CPUType>(processorCombo->currentData().toInt());
            QString machineName = nameEdit->text();
            size_t memorySize = memorySizeCombo->currentData().toULongLong();
            profile = MachineProfile(cpuType, machineName, memorySize);
            accept();
        }

    private:
        QLineEdit* nameEdit;
        QComboBox* processorCombo;
        QComboBox* memorySizeCombo;
        QPushButton* okButton;
        QPushButton* cancelButton;

        MachineProfile profile;

        void onProcessorTypeChanged(int index) {
            memorySizeCombo->clear();
            CPUType selectedCPUType = static_cast<CPUType>(processorCombo->currentData().toInt());

            if(selectedCPUType == CPUType::I8080) {
                memorySizeCombo->addItem("16KB", 16384);
                memorySizeCombo->addItem("24KB", 24576);
                memorySizeCombo->addItem("32KB", 32768);
                memorySizeCombo->addItem("48KB", 49152);
                memorySizeCombo->addItem("64KB", 65536);
            } else if(selectedCPUType == CPUType::I8085) {
                memorySizeCombo->addItem("16KB", 16384);
                memorySizeCombo->addItem("24KB", 24576);
                memorySizeCombo->addItem("32KB", 32768);
                memorySizeCombo->addItem("48KB", 49152);
                memorySizeCombo->addItem("64KB", 65536);
            } else if(selectedCPUType == CPUType::I8086) {
                memorySizeCombo->addItem("128KB", 131072);
                memorySizeCombo->addItem("256KB", 262144);
                memorySizeCombo->addItem("512KB", 524288);
                memorySizeCombo->addItem("640KB", 655360);
                memorySizeCombo->addItem("1MB", 1048576);
            } else if(selectedCPUType == CPUType::I8088) {
                memorySizeCombo->addItem("128KB", 131072);
                memorySizeCombo->addItem("256KB", 262144);
                memorySizeCombo->addItem("512KB", 524288);
                memorySizeCombo->addItem("640KB", 655360);
                memorySizeCombo->addItem("1MB", 1048576);
            } else if(selectedCPUType == CPUType::Z80) {
                memorySizeCombo->addItem("16KB", 16384);
                memorySizeCombo->addItem("32KB", 32768);
                memorySizeCombo->addItem("64KB", 65536);
                memorySizeCombo->addItem("128KB", 131072);
            }
        }
};
