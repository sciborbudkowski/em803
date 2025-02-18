#pragma once

#include "MachineProfile.h"

#include <QObject>
#include <QVector>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <iostream>

class MachinesManager : public QObject {
    Q_OBJECT

    public:
        explicit MachinesManager(QObject* parent = nullptr) : QObject(parent) {
            QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            QDir dir;
            if(!dir.exists(configDir)) {
                dir.mkpath(configDir);
            }

            filePath = configDir + "/machines.json";
            loadMachines();
        }

        bool loadMachines() {
            QFile file(filePath);
            if(!file.exists()) return true;
            if(!file.open(QIODevice::ReadOnly)) return false;

            QByteArray data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if(!doc.isArray()) return false;

            QJsonArray array = doc.array();
            machines.clear();
            for(const QJsonValue& val : array) {
                if(val.isObject()) {
                    QJsonObject obj = val.toObject();
                    QString processorType = obj["processorType"].toString();
                    QString machineName = obj["machineName"].toString();
                    QString memorySize = obj["memorySize"].toString();
                    machines.append(MachineProfile(MachineProfile::getCPUType(processorType), machineName, memorySize.toInt()));
                }
            }

            return true;
        }

        bool saveMachines() {
            QFile file(filePath);
            if(!file.open(QIODevice::WriteOnly)) return false;
            std::cout << "Saving machines to " << filePath.toStdString() << std::endl;

            QJsonArray array;
            for(const auto& machine : machines) {
                QJsonObject obj;
                obj["processorType"] = MachineProfile::getCPUName(machine.getProcessorType());
                obj["machineName"] = machine.getMachineName();
                obj["memorySize"] = QString::number(machine.getMemorySize());
                array.append(obj);
            }

            QJsonDocument doc(array);
            file.write(doc.toJson());
            file.close();

            return true;
        }

        void removeMachine(const QString& machineName) {
            for(int i=0; i<machines.size(); i++) {
                if(machines[i].getMachineName() == machineName) {
                    machines.removeAt(i);
                    break;
                }
            }
        }

        void addMachine(const MachineProfile& profile) { machines.append(profile); }

        QVector<MachineProfile> getMachines() const { return machines; }

    private:
        QVector<MachineProfile> machines;
        QString filePath;
};
