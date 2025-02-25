#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

class RegisterViewWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(quint16 value READ value WRITE setValue)
    Q_PROPERTY(bool is8Bit READ is8Bit WRITE setIs8Bit)

    private:
        QLabel* m_label;
        QLineEdit* m_lineEdit;
        QString m_labelString;
        bool m_is8Bit;

    public:
        explicit RegisterViewWidget(const QString& label, QWidget* parent = nullptr, bool is8Bit = true) 
        : QWidget(parent), m_labelString(label), m_is8Bit(is8Bit) {
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(5);

            m_label = new QLabel(this);
            layout->addWidget(m_label);
            setLabel(label);

            m_lineEdit = new QLineEdit(this);
            m_lineEdit->setReadOnly(true);
            m_lineEdit->setFixedWidth(50);
            layout->addWidget(m_lineEdit);

            setValue(0);
        }

        QString label() const { return m_label->text(); }

        void setLabel(const QString& text) {
            QString labelText = text;
            if(!labelText.isEmpty() && !labelText.endsWith(':')) labelText += ':';
            m_label->setText(labelText);
        }

        quint16 value() const {
            bool ok;
            return m_lineEdit->text().toUInt(&ok, 16);
        }

        void setValue(quint16 val) {
            if(m_is8Bit) {
                val &= 0xFF;
                m_lineEdit->setText(QString("0x%1").arg(val, 2, 16, QChar('0').toUpper()));
            } else {
                m_lineEdit->setText(QString("0x%1").arg(val, 4, 16, QChar('0').toUpper()));
            }
        }

        bool is8Bit() const { return m_is8Bit; }

        void setIs8Bit(bool is8Bit) {
            m_is8Bit = is8Bit;
            setValue(value());
        }
};
