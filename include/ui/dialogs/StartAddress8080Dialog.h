#pragma once

#include <cstdint>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

class StartAddress8080Dialog : public QDialog {
    Q_OBJECT

    private:
        QLineEdit* startAddressEdit;
        QPushButton* okButton;
        QPushButton* cancelButton;

        uint16_t startAddress;
        QString startAddressAsHexString;

        static const uint16_t defaultStartAddress = 0x0100;

        void convertStartAddressToHexString() {
            char buffer[6];

            snprintf(buffer, sizeof(buffer), "%04X", startAddress);
            startAddressAsHexString = QString::fromUtf8(buffer);
        }

        void convertHexStringToStartAddress() {
            startAddress = startAddressAsHexString.toUInt(nullptr, 16);
        }

    public:
        explicit StartAddress8080Dialog(QWidget* parent = nullptr, uint16_t startAddress = StartAddress8080Dialog::defaultStartAddress)
        : QDialog(parent), startAddress(startAddress) {
            setWindowTitle("8080 Start Address");
            convertStartAddressToHexString();

            auto* layout = new QVBoxLayout(this);
            auto* startAddressLabel = new QLabel("Start address (hex):", this);
            auto* buttonsLayout = new QHBoxLayout();

            startAddressEdit = new QLineEdit(startAddressAsHexString, this);
            okButton = new QPushButton("OK", this);
            cancelButton = new QPushButton("Cancel", this);

            buttonsLayout->addWidget(okButton);
            buttonsLayout->addWidget(cancelButton);
            layout->addWidget(startAddressLabel);
            layout->addWidget(startAddressEdit);
            layout->addLayout(buttonsLayout);

            connect(okButton, &QPushButton::clicked, this, &StartAddress8080Dialog::onOkButtonClicked);
            connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        }

        ~StartAddress8080Dialog() = default;

        uint16_t getStartAddress() const { return startAddress; }

    private slots:
        void onOkButtonClicked() {
            QString text = startAddressEdit->text().trimmed();
            bool convertible;
            
            startAddress = text.toUInt(&convertible, 16);
            if(text.isEmpty() || !convertible) {
                QMessageBox::warning(this, "Error", "Start address can not be empty and must be hex.");
                return;
            }

            accept();
        }
};
