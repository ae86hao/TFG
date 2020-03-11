/********************************************************************************
** Form generated from reading UI file 'deviceDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICEDIALOG_H
#define UI_DEVICEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_DeviceDialog
{
public:
    QRadioButton *gtec;
    QRadioButton *emotiv;
    QPushButton *apply;

    void setupUi(QDialog *DeviceDialog)
    {
        if (DeviceDialog->objectName().isEmpty())
            DeviceDialog->setObjectName(QStringLiteral("DeviceDialog"));
        DeviceDialog->resize(400, 300);
        gtec = new QRadioButton(DeviceDialog);
        gtec->setObjectName(QStringLiteral("GTec"));
        gtec->setGeometry(QRect(30, 30, 121, 22));
        gtec->setChecked(true);
        emotiv = new QRadioButton(DeviceDialog);
        emotiv->setObjectName(QStringLiteral("Emotiv"));
        emotiv->setGeometry(QRect(30, 70, 117, 22));
        apply = new QPushButton(DeviceDialog);
        apply->setObjectName(QStringLiteral("Apply"));
        apply->setGeometry(QRect(288, 240, 71, 27));

        retranslateUi(DeviceDialog);

        QMetaObject::connectSlotsByName(DeviceDialog);
    } // setupUi

    void retranslateUi(QDialog *DeviceDialog)
    {
        DeviceDialog->setWindowTitle(QApplication::translate("DeviceDialog", "Device", 0));
        gtec->setText(QApplication::translate("DeviceDialog", "GTec", 0));
        emotiv->setText(QApplication::translate("DeviceDialog", "Emotiv", 0));
        apply->setText(QApplication::translate("DeviceDialog", "Apply", 0));
    } // retranslateUi

};

namespace Ui {
    class DeviceDialog: public Ui_DeviceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICEDIALOG_H
