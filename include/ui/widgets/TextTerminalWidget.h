#pragma once

#include <QWidget>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QTimer>
#include <memory>
#include <string>
#include <vector>

#include "ITerminalAccess.h"
#include "Properties.h"

class TextTerminalWidget : public QWidget, protected QOpenGLFunctions {
    Q_OBJECT

    private:
        std::shared_ptr<ITerminalAccess> terminal;

        int widthOfCharInPixels, heightOfCharInPixels;

        QFont font;
        QTimer* timer;
        float fontSize;

        void updateScalling() {
            widthOfCharInPixels = terminal->getPixelDimension().first/terminal->getCharDimension().first;
            heightOfCharInPixels = terminal->getPixelDimension().second/terminal->getCharDimension().second;
            fontSize = static_cast<float>(heightOfCharInPixels);
        }

    public:
        TextTerminalWidget(std::shared_ptr<ITerminalAccess> terminal, QWidget* parent = nullptr)
        : QWidget(parent), terminal(terminal) {
            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, [this]() { update(); });
            timer->start(16);
            updateScalling();
            font = QFont("DejaVuSansMono");
            font.setPointSizeF(fontSize);
            
            setAttribute(Qt::WA_OpaquePaintEvent);
        }

    protected:
        void paintEvent(QPaintEvent* event) override {
            if(!terminal) {
                return;
            }

            int widthPixels = terminal->getPixelDimension().first;
            int heightPixels = terminal->getPixelDimension().second;
            int widthChars = terminal->getCharDimension().first;
            int heightChars = terminal->getCharDimension().second;
            int cursorX = terminal->getCursorPosition().first;
            int cursorY = terminal->getCursorPosition().second;
            std::vector<std::string> buffer = terminal->getBuffer();

            QPainter painter(this);
            painter.setRenderHint(QPainter::TextAntialiasing);
            painter.setFont(font);

            float offsetX = 20.0f, offsetY = 20.0f;

            painter.fillRect(offsetX, offsetY, widthPixels, heightPixels, TERMINAL_BACKGROUND_QT_COLOR);

            for(int y=0; y<heightChars; y++) {
                for(int x=0; x<widthChars; x++) {
                    char ch = buffer[y][x];
                    if(ch != ' ') {
                        float posX = offsetX + x*widthOfCharInPixels;
                        float posY = offsetY + (y + 1)*heightOfCharInPixels;

                        painter.setPen(TERMINAL_FOREGROUND_QT_COLOR);
                        painter.drawText(posX, posY, QString(ch));
                    }
                }
            }

            painter.fillRect(offsetX + cursorX*widthOfCharInPixels,
                             offsetY + cursorY*heightOfCharInPixels,
                             widthOfCharInPixels,
                             heightOfCharInPixels,
                             TERMINAL_FOREGROUND_QT_COLOR
            );
        }
};
