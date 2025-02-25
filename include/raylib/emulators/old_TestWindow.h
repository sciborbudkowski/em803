#pragma once

#include <atomic>
#include <iostream>
#include <QLabel>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include <QLayout>

class TestWindow : public QMainWindow {
    Q_OBJECT

    private:
        std::atomic<bool> isRunning;

        QTimer* timer;
        QLabel* label;

    public:
        explicit TestWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
            timer = new QTimer(this);
            isRunning.store(true, std::memory_order_relaxed);
            setWindowTitle("Test Window");
            resize(800, 600);

            QWidget* centralWidget = new QWidget(this);
            QVBoxLayout* layout = new QVBoxLayout(centralWidget);

            label = new QLabel;
            QPushButton* button = new QPushButton;
            button->setText("Stop");

            connect(timer, &QTimer::timeout, this, &TestWindow::showStatus);
            connect(button, &QPushButton::click, this, &TestWindow::stop);

            layout->addWidget(label);
            layout->addWidget(button);
            centralWidget->setLayout(layout);

            setCentralWidget(centralWidget);
        }

        ~TestWindow() = default;

        void run() {
            show();
            emulation();
        }

    private slots:
        void showStatus() {
            // label->setText("Sprawdzanie otwierania okna... " + isRunning); 
            std::cout << "showStatus" << std::endl;
        }

        void stop() { isRunning.store(false, std::memory_order_relaxed); }

        void emulation() {
            while(isRunning.load(std::memory_order_relaxed)) {
                QThread::msleep(1);
            }

            emit emulationFinished();
        }

    signals:
        void emulationFinished();
};
