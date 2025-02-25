#ifndef SRC_MAIN_CPP
#define RAYGUI_IMPLEMENTATION
#define SRC_MAIN_CPP
#endif

#include "MainWindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QMessageBox>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    int fontId = QFontDatabase::addApplicationFont(":/assets/fonts/DejaVuSansMono.ttf");
    if(fontId == -1) {
        QMessageBox::warning(nullptr, "Error", "Error loading mono font.");
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        app.setFont(QFont(fontFamily));
    }

    MainWindow w;
    w.setWindowTitle("em80 - Machine Manager");
    w.resize(300, 400);
    w.show();

    return app.exec();
}
