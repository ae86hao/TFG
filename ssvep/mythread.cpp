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


#include "mythread.h"
#include "mainwindow.h"
#include "console.h"

//La variable idCola se recibe desde el Main
extern int idCola;

//Los valores de frecuencias configuradas se reciben desde MainWindow
extern int frecuencia1_int;
extern int frecuencia2_int;

extern int frecuencia4_int;
extern int frecuencia5_int;

void MyThread::run(){

    //Declaración de variables
    MensajeRobot mensaje;
    int errorTransmision;
    QString idCola_string = QString::number(idCola);
    //Se verifica que el id de Cola es el mismo en Main y en MyThread
    qDebug("El id de cola en el hilo es " + idCola_string.toLatin1());

    //El hilo lee de la cola indefinidamente e interpreta los resultados
    while(1){
        // Lectura del mensaje de la cola
        errorTransmision = msgrcv(idCola, &mensaje, sizeof(mensaje), 0, MSG_NOERROR);
        if (errorTransmision < 0) {
            qDebug("Recepcion de datos fallida ");
        }

        // Se analiza la información según el tipo de dato almacenado en la cola
        switch(mensaje.tipoComunicacion){

        case IPC_DATOS: // Mensaje de datos
        {
            //Recepción del mensaje
            int dato = mensaje.ComandoRobot[0];
            QString dato_string = QString::number(dato);
            qDebug("La frecuencia recibida es " + dato_string.toLatin1());

            //Se realiza la comparación de frecuencia
            if (frecuencia1_int==dato){                
                qDebug("SEÑAL EMITIDA!!!");
                emit Signal_Thread_Estimulo1();
            }
            else if (frecuencia2_int==dato){
                qDebug("SEÑAL EMITIDA!!!");
                emit Signal_Thread_Estimulo2();
            }
            else if (frecuencia4_int==dato){
                qDebug("SEÑAL EMITIDA!!!");
                emit Signal_Thread_Estimulo4();
            }
            else if (frecuencia5_int==dato){
                qDebug("SEÑAL EMITIDA!!!");
                emit Signal_Thread_Estimulo5();
            }
            break;
        }
        case IPC_CONFIG: // Mensaje para configurar arduino
        {
            //si la frecuencia es 0, indica una luz fija
            QString frecuencias =  QString("99 %1 %2 %3 %4").arg(mensaje.ComandoRobot[0]).arg(mensaje.ComandoRobot[1]).arg(mensaje.ComandoRobot[2]).arg(mensaje.ComandoRobot[3]);
            emit ConfigurarLED(frecuencias);
            break;
        }
        case IPC_CONFIG_FIN: // Mensaje para configurar arduino
        {
            QString frecuencias = "66";
            emit ConfigurarLED(frecuencias);
            break;
        }
        case IPC_COMM: // Mensaje para descanso
        {
            emit DescansoUsuario(mensaje.ComandoRobot[0], mensaje.ComandoRobot[1]);
            break;
        }
        case IPC_POS: // Mensaje para determinar la posicion de la mirada del usuario
        {
            int dato = mensaje.ComandoRobot[0];
            emit SetStimPos(dato);
            break;
        }
        case IPC_POS_FIN:
        {
            emit FinSetStimPos();
            break;
        }
        case IPC_BASE_FIN:
        {
            emit FinBaseline();
            break;
        }
        case IPC_BARRIDO_FIN:
        {
            emit FinBarrido();
            break;
        }
        case IPC_SELECT_FIN:
        {
            emit FinSelectFreqs();
            break;
        }
    }
  }
}

