/** **************************************
  *
  * Autor:          Álvaro Morán García
  * Colaboración:   Jacobo Fernández Vargas (ACL)
  *
  * Fecha: 30-Octubre-2014
  *
  * Interfaz EEG bajo Emotiv Epoc - SSVEPs
  *
  * Proyecto de fin de carrera
  * Ingeniería Superior de Telecomunicación
  *
  * Universidad Autónoma de Madrid
  *
  *************************************** */

// Llamadas al sistema
#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>

// Librerías para control de tiempo y procesos
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <errno.h>
#include <sys/wait.h>

// Librerías matemáticas necesarias y de entrada y salida de datos
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_fft_real.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>

// Constantes para la gestión de directorios y ficheros
#define LONGITUD_RUTA 300
#define DIM_ARGUMENTOS 15

#define RUTA_USUARIO "/home/gnb/Hao_Zhu/ejecucion/datos/Usuario_"
#define SEPARADOR "/"

// .cfg es un fichero de configuración
#define RUTA_ESTIMULOS "detecciones"
#define RUTA_BRUTOS_ESTIMULOS "brutosEstimulos"
#define RUTA_BASELINE "baseline"
#define RUTA_BRUTOS_BASELINE "brutosBaseline"
#define RUTA_BRUTOS_BARRIDO "frecuencia_"
#define RUTA_SELECCION_REF "seleccionReferencias"
#define RUTA_BARRIDO_FREQS "barrido"
#define RUTA_SELECT_FREQS "seleccion"
#define RUTA_SECUENCIA_2 "secuencia2.cfg"
#define RUTA_SECUENCIA_4 "secuencia4.cfg"
#define RUTA_CONFIGURACION "configAnalisis.cfg"

// Número máximo de estímulos
#define MAX_ESTIM 4

// Características propias del análisis
#define MASTER "UB-2015.05.23"
#define MUESTREO 128
#define NUM_CANALES 16
#define VENTANA  2*MUESTREO
#define TIMEWINDOW 2
#define DESPL MUESTREO/4        //250ms de desplazamiento implica 1/4 s
#define SIMTIME_BASELINE 30
#define SIMTIME_FREQ 4
#define SIMTIME_ACL 28
#define SIMTIME_REAL 600
#define FREC_INI 20      // Rango de frecuencias para hacer la FFT (FREC_INI siempre DEBE ser 1Hz menor que FREC_INI_REAL para analizar adyacentes)
#define FREC_FIN 30
#define FREC_INI_REAL 20 // Rango de frecuencias utillizables por el usuario
#define FREC_FIN_REAL 30

// Constantes matemáticas a aplicar para el análisis
#define PI 3.14159265
#define PI2 2*PI

// Definición de cómo acceder a valores reales e imaginarios del vector devuelto por gsl_fft_real_radix2_transform
#define REAL(z,i) ((z)[i])
#define IMAG(z,i) ((z)[(VENTANA)-(i)])

// Constantes para el algoritmo
#define ALFA 1.5    //Puntuaciones
#define BETA 1      //Distancias
#define DELTA 3   //Ratio
#define GAMMA 0.02  //Tiempo

#define UMBRAL 0.7
#define UMBRAL_DETEC 2
#define DETECCION 7
#define MAX_DURACION 16

#define STIMPOS 100
// Constantes para la creación de la cola de mensajes
#define IPC_PERMISOS 0600
#define IPC_MENSAJE_DATOS 1
#define IPC_MENSAJE_FIN_TRANSMISION 9
#define IPC_ROBOT_DATOS 1
#define IPC_ROBOT_CONFIG 3
#define IPC_ROBOT_CONFIG_FIN 4
#define IPC_ROBOT_COMM 5
#define IPC_ROBOT_POS 7
#define IPC_ROBOT_POS_FIN 8
#define IPC_ROBOT_BASE_FIN 9
#define IPC_ROBOT_BARRIDO_FIN 10
#define IPC_ROBOT_SELECT_FIN 12
#define IPC_STOP 999

typedef enum {EMOTIV, GTEC} device;

// analysis data structs
typedef struct{
    double puntuacion[FREC_FIN-FREC_INI];   //PUNT            //Puntuación de los grupos
    double distancia[FREC_FIN-FREC_INI][FREC_FIN-FREC_INI];// DIST   //Distancia entre grupos
    int    grupos[FREC_FIN-FREC_INI][MAX_ESTIM];                  //Grupos
    int    frecuenciasGrupo; // F_ACT                         //Numero de frecuencias en grupo
    int    numeroGrupos;    // N_CONJ                         //Numero de grupos
    int    frecuenciasUtilizadas[FREC_FIN-FREC_INI];
}algoritmoDatos;

typedef struct{
    double puntuacion;
    int frecuencia;
    int segundosDeteccion;
}puntuacionesAlgoritmo;

typedef struct{
    double baselineResultado[FREC_FIN-FREC_INI];    // Resultado obtenido del baseline (todos los segundos)
    double estimuloResultado[FREC_FIN-FREC_INI];    // Resultado obtenido cada ventana para el estimulo
    int    iteraciones;                             // Iteraciones acumuladas del analisis
    double vectorAnalisisSolapado[VENTANA];         // Vector de datos a analizar
    double ventanaAnterior[VENTANA];                // Ventana de la muestra anterior a considerar
}datosAnalisis;

// communication structs
typedef struct
{
  long tipoComunicacion;
  double texto;
} MensajeIPC;
typedef struct
{
    long tipoComunicacion;
    int ComandoRobot[MAX_ESTIM];

}EnvioMensajeRobot;

typedef struct
{
    int sampleCounter;
    int secondCounter;
    int idCola;
    int idColaStop;
    int segundosSimulacion;
    int apagado;
    int numCanales;
}parameters;

struct bestRefers
{
    int freq;
    int firstEle;
    int secondEle;
    double snr;

    bool operator==(const bestRefers& a) const
    {
        return (freq == a.freq);
    }
};

using namespace std;

// Declaración de funciones
unsigned int sleep(unsigned int __seconds);
void crearRutaDirectorio( int idUsuario, char* ruta);
void detrend(double *vector, int size);
void enventanadoHanning(double *vector, int size);
void DTFT(double* vector, double *resultado, int size);
void preprocesado(double* vector, double *resultado, int size);
void analisisBaseline(datosAnalisis* procesado);
void analisisAlgoritmo(datosAnalisis* procesado);
void puntuacionFrecuenciaAlgoritmo(datosAnalisis *datos, double *puntuacion, int *frecuenciaDetectada);
void obtenerVectoresAnalisis(double *canalesDeseados, double **matrizDatos, bestRefers *br);
void obtenerVentanaAnterior(datosAnalisis *datos);
void obtenerVentanaSolapada(datosAnalisis *datos, double vectorDatos[MUESTREO], int iteracion);
int algoritmoVecinoProximo(datosAnalisis *datos, double &puntuacion);

void distancia(algoritmoDatos *datos);
void actualizar(algoritmoDatos *datos,int x, int y,double time, double rate);
void maxmat(algoritmoDatos *datos,int *x,int *y);
void selmaxmat(algoritmoDatos *datos,int *x,int *y);
void cambiar(algoritmoDatos *datos);


