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
** MainWindow
**  Interfaz Gráfica de Usuario. Aplicación que se comporta como Interfaz Cerebro -
**  Máquina, comunicándose tanto con el casco Emotiv como con Arduino.
**
*******************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define num_estimulos 4

//cadena de caracteres para speller
#define CADENA "ABCDEFGHIJKLMNÑOPQRSTUVWXYZ"

//////////////////////////////////
/// Constantes para fijar Objetos
//////////////////////////////////

//Parámetros de configuración para el monitor del GNB
#define ancho_pantalla 1900
#define alto_pantalla 1020

//Monitor de un PC portatil
//#define ancho_pantalla 1250 // 1350 pantalla pequeña
//#define alto_pantalla 950// 700 pantalla pequeña

#define margen_superior 40// 20 pantalla pequeña
#define margen_lateral 20 // 15 pantalla pequeña

#define distancia_botones ancho_pantalla/3

#define ancho_estimulo (ancho_pantalla/3)-2*margen_lateral
#define alto_estimulo (alto_pantalla/2)-2*margen_superior - margen_superior/2


//////////////////////////////////
/// Rutas a Ficheros de Control
//////////////////////////////////

// Fichero que almacena los usuarios registrados en el sistema
#define RUTA_RegUsus "/home/gnb/Hao_Zhu/ejecucion/datos/RegUsus.txt"
#define RUTA_USUARIO "/home/gnb/Hao_Zhu/ejecucion/datos/Usuario_"

//////////////////////////////////
/// Constantes para la creación de la cola de mensajes
//////////////////////////////////

#define IPC_LONGITUD 128
#define IPC_PERMISOS 0600
#define IPC_MENSAJE_CABECERA 2
#define IPC_MENSAJE_DATOS 1
#define IPC_MENSAJE_FIN_TRANSMISION 9
#define IPC_ROBOT_DATOS 1
#define IPC_ROBOT_FIN 2

#define TOPL 100
#define TOPR 101
#define BOTL 102
#define BOTR 103

//////////////////////////////////
/// Programas que han de ejecutarse en paralelo a la GUI
//////////////////////////////////

#define ARCHIVO_EJECUCIONEMOTIV "/home/gnb/Hao_Zhu/ejecucion/ejecucionEmotiv"
#define ARCHIVO_EJECUCIONGTEC "/home/gnb/Hao_Zhu/ejecucion/ejecucionGTec"

//////////////////////////////////
/// Librerías
//////////////////////////////////

#include <QtCore/QtGlobal>
#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QFile>
#include <QPushButton>
#include <QElapsedTimer>
#include <QMutex>
#include <QPixmap>
#include <QMatrix>
#include <QDir>

//////////////////////////////////
/// Signals, slots y funciones. Implementación de la clase MainWindow
//////////////////////////////////

QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;
class DeviceDialog;
class MyThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    static MainWindow * getInstance();
    ~MainWindow();

signals:
    //Señales que se emiten cuando se detecta un estímulo
    void Signal_Estimulo1();
    void Signal_Estimulo2();

    void Signal_Estimulo4();
    void Signal_Estimulo5();


private slots:
    void openSerialPort();
    void closeSerialPort();
    void chooseDevice();
    void about();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    // Funciones que se activan cuando se detecta un estímulo

    void Estimulo1Detectado();
    void Estimulo2Detectado();
    void Estimulo4Detectado();
    void Estimulo5Detectado();
    void ConfigLEDDetectado(QString);
    void DescansoDetectado(int, int);
    void SetStimPosDetectado(int);
    void FinSetStimPosDetectado();
    void FinBaselineDetectado();
    void FinBarridoDetectado();
    void FinSelectFreqsDetectado();


    //Funciones de llamada de los botones

    void on_boton_inicio_clicked();
    void on_Boton_NuevoUsu_clicked();
    void on_Boton_RegUsu_clicked();


private:
    static bool instanceFlag;
    static MainWindow * myinstance;
    void initActionsConnections();

    //Funciones de configuración de estados de la GUI
    void setInicio();
    void setPosiciones();
    void checkEstados(int estados);
    void setEstado0();
    void showSpeller(QList<int> sec, QString cad);


private:
    void showStatusMessage(const QString &message);
    Ui::MainWindow *ui;
    QLabel *status;
    Console *console;
    SettingsDialog *settings;
    DeviceDialog *device;
    QSerialPort *serial;

    int contador_inicio;

    QString numero_usuario;
    QString nuevo_usuario;
    int int_numero_usuario;
    QString accion;
    QString numCanales;

    QString frecuencias_completas;
    QString frecuencia1;
    QString frecuencia2;

    QString frecuencia4;
    QString frecuencia5;

    QString executable;

    QString cadena_speller;
    QString nombre_usuario;

    QStringList lista_frecuencias;
    QStringList lista_usuarios;

    QElapsedTimer temporizador_baseline;

    //Control de estados
    int estado;
    int estado_NU;
    int estado_UE;
    bool existe_usuario;

    //Juegos
    int contador12luces;
    int mirarluz;
    QElapsedTimer temporizador12l;
    int numero_fallos;

};

#endif // MAINWINDOW_H
