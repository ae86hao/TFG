/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionAboutQt;
    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionConfigure;
    QAction *actionDevice;
    QAction *actionClear;
    QAction *actionQuit;

    QWidget *centralWidget;
    QPushButton *boton_inicio;
    QWidget *estimulo1;
    QList<QLabel*> *labels;
    QPushButton *pushButton_1;
    QWidget *estimulo2;

    QPushButton *pushButton_2;

    QWidget *estimulo4;

    QPushButton *pushButton_4;
    QWidget *estimulo5;

    QPushButton *pushButton_5;

    QLabel *Bienvenido;
    QLabel *InicieSesion;
    QPushButton *Boton_NuevoUsu;
    QPushButton *Boton_RegUsu;
    QLabel *label_introducenombre;
    QLabel *label_InfoNuevoUsu;
    QLabel *label_arrow;
    QLineEdit *line_NombreUsu;
    QMenuBar *menuBar;
    QMenu *menuCalls;
    QMenu *menuTools;
    QMenu *menuHelp;
    QMenu *menuStart;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1138, 778);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAboutQt = new QAction(MainWindow);
        actionAboutQt->setObjectName(QStringLiteral("actionAboutQt"));

        actionConnect = new QAction(MainWindow);
        actionConnect->setObjectName(QStringLiteral("actionConnect"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/connect.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionConnect->setIcon(icon);

        actionDisconnect = new QAction(MainWindow);
        actionDisconnect->setObjectName(QStringLiteral("actionDisconnect"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDisconnect->setIcon(icon1);

        actionConfigure = new QAction(MainWindow);
        actionConfigure->setObjectName(QStringLiteral("actionConfigure"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionConfigure->setIcon(icon2);

        actionDevice = new QAction(MainWindow);
        actionDevice->setObjectName(QStringLiteral("actionDevice"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/device.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDevice->setIcon(icon3);

        actionClear = new QAction(MainWindow);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClear->setIcon(icon4);

        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/application-exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon5);

        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setStyleSheet(QStringLiteral("background-color: rgb(102, 102, 102);"));
        boton_inicio = new QPushButton(centralWidget);
        boton_inicio->setObjectName(QStringLiteral("boton_inicio"));
        boton_inicio->setGeometry(QRect(9, 501, 130, 23));
        boton_inicio->setStyleSheet(QLatin1String("background-color: rgb(227, 218, 218);\n"
"color: rgb(17, 7, 7);"));

        labels = new QList<QLabel*>();
        estimulo1 = new QWidget(centralWidget);
        estimulo1->setObjectName(QStringLiteral("estimulo1"));
        estimulo1->setGeometry(QRect(19, 10, 241, 201));
        estimulo1->setStyleSheet(QLatin1String("background-color: rgb(62, 178, 96);\n"
"border-color: rgb(29, 24, 24);"));
        QLabel *l1 = new QLabel(estimulo1);
        labels->append(l1);
        labels->at(0)->setObjectName(QStringLiteral("label_1"));
        labels->at(0)->setGeometry(QRect(70, 20, 161, 71));
        labels->at(0)->setStyleSheet(QStringLiteral(""));
        pushButton_1 = new QPushButton(estimulo1);
        pushButton_1->setObjectName(QStringLiteral("pushButton_1"));
        pushButton_1->setGeometry(QRect(110, 140, 75, 23));
        pushButton_1->setStyleSheet(QStringLiteral("background-color: rgb(160, 160, 160);"));
        estimulo2 = new QWidget(centralWidget);
        estimulo2->setObjectName(QStringLiteral("estimulo2"));
        estimulo2->setGeometry(QRect(280, 10, 241, 201));
        estimulo2->setStyleSheet(QStringLiteral("background-color: rgb(255, 170, 0);"));
        QLabel *l2 = new QLabel(estimulo2);
        labels->append(l2);
        labels->at(1)->setObjectName(QStringLiteral("label_2"));
        labels->at(1)->setGeometry(QRect(70, 20, 161, 71));
        labels->at(1)->setStyleSheet(QStringLiteral(""));
        pushButton_2 = new QPushButton(estimulo2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(110, 140, 75, 23));
        pushButton_2->setStyleSheet(QStringLiteral("background-color: rgb(160, 160, 160);"));

        estimulo4 = new QWidget(centralWidget);
        estimulo4->setObjectName(QStringLiteral("estimulo4"));
        estimulo4->setGeometry(QRect(20, 240, 241, 201));
        estimulo4->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 127);"));
        QLabel *l4 = new QLabel(estimulo4);
        labels->append(l4);
        labels->at(2)->setObjectName(QStringLiteral("label_4"));
        labels->at(2)->setGeometry(QRect(70, 20, 161, 71));
        labels->at(2)->setStyleSheet(QStringLiteral(""));
        pushButton_4 = new QPushButton(estimulo4);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(110, 140, 75, 23));
        pushButton_4->setStyleSheet(QStringLiteral("background-color: rgb(160, 160, 160);"));
        estimulo5 = new QWidget(centralWidget);
        estimulo5->setObjectName(QStringLiteral("estimulo5"));
        estimulo5->setGeometry(QRect(280, 240, 241, 201));
        estimulo5->setStyleSheet(QStringLiteral("background-color: rgb(85, 255, 255);"));
        QLabel *l5 = new QLabel(estimulo5);
        labels->append(l5);
        labels->at(3)->setObjectName(QStringLiteral("label_5"));
        labels->at(3)->setGeometry(QRect(70, 20, 161, 71));
        labels->at(3)->setStyleSheet(QStringLiteral(""));
        pushButton_5 = new QPushButton(estimulo5);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setGeometry(QRect(110, 140, 75, 23));
        pushButton_5->setStyleSheet(QStringLiteral("background-color: rgb(160, 160, 160);"));

        Bienvenido = new QLabel(centralWidget);
        Bienvenido->setObjectName(QStringLiteral("Bienvenido"));
        Bienvenido->setGeometry(QRect(220, 480, 531, 81));
        Bienvenido->setStyleSheet(QLatin1String("font: 32pt \"Ubuntu\";\n"
"color: rgb(154, 234, 185);"));
        InicieSesion = new QLabel(centralWidget);
        InicieSesion->setObjectName(QStringLiteral("InicieSesion"));
        InicieSesion->setGeometry(QRect(120, 570, 741, 81));
        InicieSesion->setStyleSheet(QLatin1String("font: 32pt \"Ubuntu\";\n"
"color: rgb(154, 234, 185);"));
        Boton_NuevoUsu = new QPushButton(centralWidget);
        Boton_NuevoUsu->setObjectName(QStringLiteral("Boton_NuevoUsu"));
        Boton_NuevoUsu->setGeometry(QRect(800, 20, 211, 41));
        Boton_NuevoUsu->setStyleSheet(QStringLiteral("background-color: rgb(212, 204, 204);"));
        Boton_RegUsu = new QPushButton(centralWidget);
        Boton_RegUsu->setObjectName(QStringLiteral("Boton_RegUsu"));
        Boton_RegUsu->setGeometry(QRect(800, 80, 211, 41));
        Boton_RegUsu->setStyleSheet(QStringLiteral("background-color: rgb(212, 204, 204);"));
        label_introducenombre = new QLabel(centralWidget);
        label_introducenombre->setObjectName(QStringLiteral("label_introducenombre"));
        label_introducenombre->setGeometry(QRect(840, 210, 211, 51));
        label_introducenombre->setStyleSheet(QLatin1String("font: 20 pt \"Ubuntu\";\n"
"color: rgb(154, 234, 185);"));
        label_InfoNuevoUsu = new QLabel(centralWidget);
        label_InfoNuevoUsu->setObjectName(QStringLiteral("label_InfoNuevoUsu"));
        label_InfoNuevoUsu->setGeometry(QRect(810, 140, 151, 41));
        label_InfoNuevoUsu->setStyleSheet(QLatin1String("font: 20 pt \"Ubuntu\";\n"
"color: rgb(154, 234, 185);"));
        label_arrow = new QLabel(centralWidget);
        label_arrow->setObjectName(QStringLiteral("label_arrow"));
        label_arrow->setGeometry(QRect(300, 50, 300, 300));
        line_NombreUsu = new QLineEdit(centralWidget);
        line_NombreUsu->setObjectName(QStringLiteral("line_NombreUsu"));
        line_NombreUsu->setGeometry(QRect(970, 150, 171, 31));
        line_NombreUsu->setStyleSheet(QStringLiteral("color: rgb(253, 253, 253);"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1138, 25));
        menuCalls = new QMenu(menuBar);
        menuCalls->setObjectName(QStringLiteral("menuCalls"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuStart = new QMenu(menuBar);
        menuStart->setObjectName(QStringLiteral("menuStart"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuStart->menuAction());
        menuBar->addAction(menuCalls->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuCalls->addAction(actionConnect);
        menuCalls->addAction(actionDisconnect);
        menuCalls->addSeparator();
        menuCalls->addAction(actionQuit);
        menuTools->addAction(actionConfigure);
        menuTools->addAction(actionDevice);
        menuTools->addAction(actionClear);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAboutQt);
        menuStart->addSeparator();
        mainToolBar->addAction(actionConnect);
        mainToolBar->addAction(actionDisconnect);
        mainToolBar->addAction(actionConfigure);
        mainToolBar->addAction(actionDevice);
        mainToolBar->addAction(actionClear);
        mainToolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Simple Terminal", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "&About", 0));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("MainWindow", "About program", 0));
#endif // QT_NO_TOOLTIP
        actionAbout->setShortcut(QApplication::translate("MainWindow", "Alt+A", 0));
        actionAboutQt->setText(QApplication::translate("MainWindow", "About Qt", 0));
        actionConnect->setText(QApplication::translate("MainWindow", "C&onnect", 0));
#ifndef QT_NO_TOOLTIP
        actionConnect->setToolTip(QApplication::translate("MainWindow", "Connect to serial port", 0));
#endif // QT_NO_TOOLTIP
        actionConnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionDisconnect->setText(QApplication::translate("MainWindow", "&Disconnect", 0));
#ifndef QT_NO_TOOLTIP
        actionDisconnect->setToolTip(QApplication::translate("MainWindow", "Disconnect from serial port", 0));
#endif // QT_NO_TOOLTIP
        actionDisconnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0));
        actionConfigure->setText(QApplication::translate("MainWindow", "&Configure", 0));
#ifndef QT_NO_TOOLTIP
        actionConfigure->setToolTip(QApplication::translate("MainWindow", "Configure serial port", 0));
#endif // QT_NO_TOOLTIP
        actionConfigure->setShortcut(QApplication::translate("MainWindow", "Alt+C", 0));
        actionDevice->setText(QApplication::translate("MainWindow", "D&evice", 0));
#ifndef QT_NO_TOOLTIP
        actionDevice->setToolTip(QApplication::translate("MainWindow", "Choose device to be used", 0));
#endif // QT_NO_TOOLTIP
        actionDevice->setShortcut(QApplication::translate("MainWindow", "Alt+E", 0));
        actionClear->setText(QApplication::translate("MainWindow", "C&lear", 0));
#ifndef QT_NO_TOOLTIP
        actionClear->setToolTip(QApplication::translate("MainWindow", "Clear data", 0));
#endif // QT_NO_TOOLTIP
        actionClear->setShortcut(QApplication::translate("MainWindow", "Alt+L", 0));
        actionQuit->setText(QApplication::translate("MainWindow", "&Quit", 0));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0));

        boton_inicio->setText(QApplication::translate("MainWindow", "Pulse aqu\303\255 para comenzar", 0));
        labels->at(0)->setText(QApplication::translate("MainWindow", "Juegos", 0));
        pushButton_1->setText(QApplication::translate("MainWindow", "Seleccionar", 0));
        labels->at(1)->setText(QApplication::translate("MainWindow", "Juegos", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "Seleccionar", 0));

        labels->at(2)->setText(QApplication::translate("MainWindow", "Juegos", 0));
        pushButton_4->setText(QApplication::translate("MainWindow", "Seleccionar", 0));
        labels->at(3)->setText(QApplication::translate("MainWindow", "Juegos", 0));
        pushButton_5->setText(QApplication::translate("MainWindow", "Seleccionar", 0));

        Bienvenido->setText(QApplication::translate("MainWindow", "Bienvenido a Brain Reader", 0));
        InicieSesion->setText(QApplication::translate("MainWindow", "Por favor, inicie sesi\303\263n para Continuar", 0));
        Boton_NuevoUsu->setText(QApplication::translate("MainWindow", "Nuevo Usuario", 0));
        Boton_RegUsu->setText(QApplication::translate("MainWindow", "Usuario Registrado", 0));
        label_introducenombre->setText(QApplication::translate("MainWindow", "Eres el usuario:", 0));
        label_InfoNuevoUsu->setText(QApplication::translate("MainWindow", "Introduce tu nombre:", 0));
        menuCalls->setTitle(QApplication::translate("MainWindow", "Calls", 0));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
        menuStart->setTitle(QApplication::translate("MainWindow", "Start", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
