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
** Clase MyThread
**  Hilo que recibe datos de la ejecución del Emotiv e interpreta los resultados,
**  enviándolos como estímulos detectados a la GUI.
**
*******************************************************************************************/


#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "mainwindow.h"

// Librerías de configuración del hilo y la cola
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>


// Constantes para la creación de la cola de mensajes
#define IPC_DATOS 1
#define IPC_CONFIG 3
#define IPC_CONFIG_FIN 4
#define IPC_COMM 5
#define IPC_POS 7
#define IPC_POS_FIN 8
#define IPC_BASE_FIN 9
#define IPC_BARRIDO_FIN 10
#define IPC_SELECT_FIN 12
#define LIMITE_CONTADOR 2 // Cada 4 positivos consecutivos se considera un acierto

//Contenedor que recibirá el mensaje almacenado en la cola que contiene el resultado
// de la ejecución del Emotiv
typedef struct
{
    long tipoComunicacion;
    int ComandoRobot[num_estimulos];

}MensajeRobot;


class MyThread : public QThread
{
    Q_OBJECT

//Mecanismo Signals & Slots: Estas señales activarán las funciones que activan los estímulos
signals:
    void Signal_Thread_Estimulo1();
    void Signal_Thread_Estimulo2();
    void Signal_Thread_Estimulo4();
    void Signal_Thread_Estimulo5();
    void ConfigurarLED(QString);
    void DescansoUsuario(int, int);
    void SetStimPos(int);
    void FinSetStimPos();
    void FinBaseline();
    void FinBarrido();
    void FinSelectFreqs();

private:
    static bool instanceFlagThread;
    static MyThread * myinstanceThread;


public:
    static MyThread* getInstance();
    void run();
};

#endif // MYTHREAD_H
