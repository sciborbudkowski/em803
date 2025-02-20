#ifndef SRC_MAIN_CPP
#define RAYGUI_IMPLEMENTATION
#define SRC_MAIN_CPP
#endif

#include "MainWindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SetTraceLogLevel(LOG_WARNING);

    MainWindow w;
    w.setWindowTitle("em80 - Machine Manager");
    w.resize(300, 400);
    w.show();

    return app.exec();
}
