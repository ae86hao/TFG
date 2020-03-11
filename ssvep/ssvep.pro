QT += widgets serialport

TARGET = ssvep
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp \
    mythread.cpp \
    deviceDialog.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    console.h \
    mythread.h \
    ui_mainwindow.h \
    ui_settingsdialog.h \
    ui_deviceDialog.h \
    ui_deviceDialog.h \
    deviceDialog.h

RESOURCES += \
    ssvep.qrc
