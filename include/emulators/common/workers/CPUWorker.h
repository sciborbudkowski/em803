#pragma once

#include <atomic>
#include <memory>
#include <QObject>
#include <QThread>

#include "ICPU.h"

class CPUWorker : public QObject {
    Q_OBJECT

    private:
        std::shared_ptr<ICPU> cpu;
        std::atomic<bool> isRunning;

    public:
        explicit CPUWorker(std::shared_ptr<ICPU> cpu, QObject* parent = nullptr)
        : QObject(parent), cpu(cpu), isRunning(false) {}

    public slots:
        void run() {
            isRunning = true;
            cpu->start();
            emit running();
        }

        void stop() {
            isRunning = false;
            cpu->stop();
            emit stopped();
        }

    signals:
        void running();
        void stopped();
};
