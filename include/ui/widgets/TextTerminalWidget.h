#pragma once

#include <QWidget>
#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QTimer>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "ITerminalAccess.h"
#include "Properties.h"

class TextTerminalWidget : public QWidget, protected QOpenGLFunctions {
    Q_OBJECT

    private:
        std::shared_ptr<ITerminalAccess> terminal;
        int widthOfCharInPixels, heightOfCharInPixels;
        int fontSize;
        bool cursorVisible = true;

        QFont font;
        QTimer* cursorTimer;
        QTimer* updateTimer;

        void updateScalling() {
            widthOfCharInPixels = terminal->getPixelDimension().first/terminal->getCharDimension().first;
            heightOfCharInPixels = terminal->getPixelDimension().second/terminal->getCharDimension().second;
        }

        void updateFontSize() {
            fontSize = 1;
            while(true) {
                font.setPointSize(fontSize);
                QFontMetrics metrics(font);

                if(metrics.horizontalAdvance('X') > widthOfCharInPixels || metrics.height() > heightOfCharInPixels) {
                    break;
                }

                fontSize++;
            }

            if(fontSize > 1) fontSize--;

            font.setPointSize(fontSize);
        }

    public:
        TextTerminalWidget(std::shared_ptr<ITerminalAccess> terminal, QWidget* parent = nullptr)
        : QWidget(parent), terminal(terminal) {
            //updateScalling();
            updateTimer = new QTimer(this);
            connect(updateTimer, &QTimer::timeout, this, [this]() { update(); });
            updateTimer->start(16);

            cursorTimer = new QTimer(this);
            connect(cursorTimer, &QTimer::timeout, this, [this]() {
                cursorVisible = !cursorVisible;
                update();
            });
            cursorTimer->start(500);
            
            setAttribute(Qt::WA_OpaquePaintEvent);
            setFocusPolicy(Qt::StrongFocus);
        }

    protected:
        void paintEvent(QPaintEvent* event) override {
            if(!terminal) {
                return;
            }

            // int widthPixels = terminal->getPixelDimension().first;
            // int heightPixels = terminal->getPixelDimension().second;
            float offsetX = 20.0f, offsetY = 20.0f;

            int widthPixels = width() - 2*offsetX;
            int heightPixels = height() - 2*offsetY;

            int widthChars = terminal->getCharDimension().first;
            int heightChars = terminal->getCharDimension().second;

            float charWitdhByWidth = static_cast<float>(widthPixels)/widthChars;
            float charHeightByHeight = static_cast<float>(heightPixels)/heightChars;
            float charSize = std::min(charWitdhByWidth, charHeightByHeight);

            widthOfCharInPixels = static_cast<int>(charWitdhByWidth);
            heightOfCharInPixels = static_cast<int>(charHeightByHeight);
            updateFontSize();

            int cursorX = terminal->getCursorPosition().first;
            int cursorY = terminal->getCursorPosition().second;
            std::vector<std::string> buffer = terminal->getBuffer();

            QPainter painter(this);
            painter.setRenderHint(QPainter::TextAntialiasing);
            painter.setFont(font);
            painter.fillRect(rect(), QColor(0, 0, 0));

            painter.fillRect(offsetX, offsetY, widthPixels, heightPixels, TERMINAL_BACKGROUND_QT_COLOR);
            if(hasFocus()) {
                painter.setPen(QPen(Qt::gray, 2));
                painter.drawRect(offsetX - 2, offsetY - 2, widthPixels + 4, heightPixels + 4);
            }

            for(int y=0; y<heightChars; y++) {
                for(int x=0; x<widthChars; x++) {
                    char ch = buffer[y][x];
                    if(ch != ' ' && ch != '\0' && ch != '\n') {
                        float posX = offsetX + x*widthOfCharInPixels;
                        float posY = offsetY + (y + 1)*heightOfCharInPixels;

                        painter.setPen(TERMINAL_FOREGROUND_QT_COLOR);
                        painter.drawText(posX, posY, QString(QChar(ch)));
                    }
                }
            }

            // cursor
            if(cursorVisible) {
                painter.fillRect(offsetX + cursorX*widthOfCharInPixels,
                                offsetY + (cursorY + 1)*heightOfCharInPixels - heightOfCharInPixels + 5,
                                widthOfCharInPixels,
                                heightOfCharInPixels + 5,
                                TERMINAL_FOREGROUND_QT_COLOR
                );
            }
        }

        void keyPressEvent(QKeyEvent* event) override {
            if(!terminal) {
                QWidget::keyPressEvent(event);
                return;
            }

            switch(event->key()) {
                case Qt::Key_Backspace:
                    terminal->backspace();
                    break;
                case Qt::Key_Return:
                case Qt::Key_Enter:
                    terminal->outputChar('\n');
                    break;
                default:
                    QString text = event->text();
                    if(!text.isEmpty()) {
                        for(QChar ch : text) {
                            if(ch.isPrint()) {
                                terminal->outputChar(ch.toLatin1());
                                terminal->setLastChar(ch.toLatin1());
                            }
                        }
                    }
            }

            update();
            event->accept();
        }

        void focusInEvent(QFocusEvent* event) override {
            QWidget::focusInEvent(event);
            update();
        }

        void focusOutEvent(QFocusEvent* event) override {
            QWidget::focusOutEvent(event);
            update();
        }

        void resizeEvent(QResizeEvent* event) override {
            QWidget::resizeEvent(event);
            update();
        }
};
