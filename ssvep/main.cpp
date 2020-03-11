/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/
/******************************************************************************************
**  Ana Isabel Sotomayor Romillo
**
**  Trabajo de Fin de Máster:
**  Control adaptado al sujeto de una interfaz cerebro-máquina con señalización mixta
**
**  Máster en Ingeniería de Telecomunicaciones
**
** ----------------------------------------------
**
** Main
**  Fichero principal que invoca al resto del programa. Crea la cola de mensajes que se
**  comunicará con el casco Emotiv y la conexión entre resultados y emisiones de estímulos
**
*******************************************************************************************/

#include <QApplication>

#include "mainwindow.h"
#include "mythread.h"
#include <sys/msg.h> //Para incluir IPC_PRIVATE, IPC_CREAT e IPC_EXCL

// Llamadas al sistema
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>


// Librerías para la entrada y salida de datos
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

// Librerías para control de tiempo y procesos
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int idCola;

int main(int argc, char *argv[])
{
    //Primero: Crear la cola de comunicación entre el casco y la GUI
    idCola = msgget(IPC_PRIVATE, IPC_PERMISOS|IPC_CREAT|IPC_EXCL);
    if (idCola < 0)
        qDebug("[WARNING] Fallo al crear la cola de comunicación con el robot");
    if(idCola > 0)
        qDebug("ID de la cola de mensajes creada");


    QApplication a(argc, argv);

    //Se crea la ventana principal de la GUI
    MainWindow w;
    //Se crea el hilo que se ejecutará en paralelo, comunicándose con el casco Emotiv
    MyThread mythread;

    // Mecanismo SIGNALS & SLOTS: Conexión entre el hilo y la GUI
    QObject::connect(& mythread, SIGNAL(Signal_Thread_Estimulo1()), & w, SLOT(Estimulo1Detectado()));
    QObject::connect(& mythread, SIGNAL(Signal_Thread_Estimulo2()), & w, SLOT(Estimulo2Detectado()));
    QObject::connect(& mythread, SIGNAL(Signal_Thread_Estimulo4()), & w, SLOT(Estimulo4Detectado()));
    QObject::connect(& mythread, SIGNAL(Signal_Thread_Estimulo5()), & w, SLOT(Estimulo5Detectado()));
    QObject::connect(& mythread, SIGNAL(ConfigurarLED(QString)), & w, SLOT(ConfigLEDDetectado(QString)));
    QObject::connect(& mythread, SIGNAL(DescansoUsuario(int, int)), & w, SLOT(DescansoDetectado(int, int)));
    QObject::connect(& mythread, SIGNAL(SetStimPos(int)), & w, SLOT(SetStimPosDetectado(int)));
    QObject::connect(& mythread, SIGNAL(FinSetStimPos()), & w, SLOT(FinSetStimPosDetectado()));
    QObject::connect(& mythread, SIGNAL(FinBaseline()), & w, SLOT(FinBaselineDetectado()));
    QObject::connect(& mythread, SIGNAL(FinBarrido()), & w, SLOT(FinBarridoDetectado()));
    QObject::connect(& mythread, SIGNAL(FinSelectFreqs()), & w, SLOT(FinSelectFreqsDetectado()));

    //Comienzan el hilo y la ventana GUI
    mythread.start();
    w.show();

    return a.exec();
}
