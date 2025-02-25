#pragma once

#include "Properties.h"
#include "CPU8080.h"
#include "ITerminalAccess.h"
#include "Terminal.h"

#include <memory>
#include <QLayout>
#include <QPushButton>

// #include <GL/gl.h>
// #include <QtOpenGLWidgets/QOpenGLWidget>
// #include <QOpenGLFunctions>
// #include <QTimer>
// #include <QPainter>
// #include <QImage>
// #include <QtOpenGLWidgets/qopenglwidget.h>
// #include <cstddef>
// #include <iostream>
// #include <qboxlayout.h>
// #include <qimage.h>
// #include <qmainwindow.h>
// #include <qobject.h>
// #include <qpainter.h>
// #include <qpushbutton.h>
// #include <qtimer.h>
// #include <qtmetamacros.h>
// #include <qwidget.h>
// #include <qwindowdefs.h>

class QtEmulatorWindow8080 : public QObject { // : public QMainWindow {
    Q_OBJECT

    private:
        CPU8080 cpu;
        std::shared_ptr<ITerminalAccess> term;

        size_t memorySize;
        bool showingMemoryView;

        QImage frameBuffer;
        QWidget* window;
        QTimer* timer;

    protected:
        // void initializeGL() override {
        //     initializeOpenGLFunctions();
        //     glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // }

        // void resizeGL(int width, int height) override { glViewport(0, 0, width, height); }

        // void paintGL() override {
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //     QPainter painter(this);
        //     if(!frameBuffer.isNull()) {
        //         painter.drawImage(rect(), frameBuffer);
        //     }
        // }

    public:
        explicit QtEmulatorWindow8080(QWidget* parent, std::shared_ptr<ITerminalAccess> terminal, size_t memorySize)
        : memorySize(memorySize), term(terminal), cpu(terminal, memorySize) {
            window = new QWidget;
            window->setParent(parent);
            window->setWindowTitle("Emulator | Intel 8080");
            window->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, [this](){
                if(window) {
                    window->update();
                }
            });
            timer->start(16);
        }

        // void updateFrameBuffer(const QImage& frameBuffer) {
        //     this->frameBuffer = frameBuffer;
        //     update();
        // }
        
        ~QtEmulatorWindow8080() {
            delete timer;
            delete window;
        };

        void run() {
            std::cout << "QtEmulatorWindow8080::run()" << std::endl;

            // Terminal* termWidget = dynamic_cast<Terminal*>(terminal);
            // if(termWidget) {
            //     setCentralWidget(termWidget);
            // } else {
            //     std::cout << "Something is wrong with casting ITerminalAccess to Terminal" << std::endl;
            // }
            QVBoxLayout* layout = new QVBoxLayout(window);
            QPushButton* b = new QPushButton("TEST", window);
            layout->addWidget(b);
            window->setLayout(layout);

            window->show();
            //while(true) {}
        }
};
