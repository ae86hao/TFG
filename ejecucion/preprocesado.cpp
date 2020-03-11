/** **************************************
  *
  * Autor:          Álvaro Morán García
  * Colaboración:   Jacobo Fernández Vargas (ACL y análisis de señal)
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

// Librerías necesarias
#include "preprocesado.h"


/**
 * @brief detrend: Función encargada de eliminar tendencias lineales de una función
 * @param vector: Array de valores que representan la función sobre la que operar por referencia.
 */
void detrend(double *vector, int size)
{
    double a=0,b,p1=0,p2=0,d,num=0,aux;
    int i;
    d=1;
    // Programación del algoritmo para eliminar componentes lineales
    for(i=0;i<(size);i++){
        p1+=i*d;
        p2+=vector[i];
    }
    p1=-p1/(size+1);
    p2=-p2/(size+1);
    for(i=0;i<size;i++){
        num+=(i*d+p1)*(vector[i]+p2);
        aux=(i*d+p1);
        a+=aux*aux;
    }
    a=num/a;
    b=a*p1-p2;
    for(i=0;i<size;i++)
        vector[i]=vector[i]-i*a-b;
}

/**
 * @brief enventanadoHanning: Función encargada de aplicar el filtro Hanning a una función
 * @param vector: Array de valores que representan la función sobre la que operar por referencia.
 */
void enventanadoHanning(double *vector, int size){
    int i;
    double hanning = PI2/(size-1);
    // Aplicamos a la cadena de entrada el filtro de Hanning
    for(i=0;i<size;i++)
        vector[i]=vector[i]*0.5*(1-cos(hanning*i));
}

/**
 * @brief DTFT: Función encargada de obtener las características frecuenciales de una función temporal aplicando detrending y filtrado
 * @param vector: Array de valores que representan la función sobre la que operar
 * @param resultado: Resultado (puntuación) obtenido para cada frecuencia por referencia.
 */
void DTFT(double* vector, double *resultado, int size){
    FILE *f;
    f=fopen("/home/gnb/Hao_Zhu/ejecucion/datos/fft", "a");
    fprintf(f, "init\n");
    // Eliminamos las componentes lineales
    detrend(vector, size);
    // Filtrado Hanning de la señal
    enventanadoHanning(vector, size);
    // Transformada de Fourier de la señal
    gsl_fft_real_radix2_transform(vector, 1, size);
    // Formamos el vector retorno con las frecuencias y el peso obtenida para cada una
    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
        // Acceso a las posiciones donde están almacenados los valores reales e imaginarios asociados a cada frecuencia y cálculo de valor absoluto
        resultado[i] = REAL(vector,(i+FREC_INI)*2)*REAL(vector,(i+FREC_INI)*2)+IMAG(vector,(i+FREC_INI)*2)*IMAG(vector,(i+FREC_INI)*2);
        //resutaldo[0] = amplitud de la frecuencia FREC_INI, respectivamente
        resultado[i] = sqrt(resultado[i]);
        fprintf(f, "%lf\t", resultado[i]);
    }
    fprintf(f, "\n");
    fclose(f);
}

/**
 * @brief preprocesado: Función encargada de lanzar el preprocesado de señal
 * @param vector: Array de valores que representan la función sobre la que operar
 * @param resultado: Resultado (puntuación) obtenido para cada frecuencia por referencia.
 */
void preprocesado(double* vector, double *resultado/*[FREC_FIN-FREC_INI]*/, int size){
    int i;
    // Inicializamos la matriz de resultados
    for(i = 0; i < FREC_FIN-FREC_INI; i++)
        resultado[i] = 0;
    // Preprocesado de la señal
    DTFT(vector, resultado, size);
}

/**
 * @brief analisisBaseline: Llamada cada ventana de análisis para obtener el procesamiento de señal (baseline) asociado a cada ventana de tiempo
 * @param procesado: Estructura conetenedora de los datos del análisis.
 */
void analisisBaseline(datosAnalisis* procesado){
    // Variables necesarias
    double resultado[FREC_FIN-FREC_INI];

    // Llamamos a la función lanzadora del preprocesado del baseline
    preprocesado(procesado->vectorAnalisisSolapado, resultado, VENTANA);

    // Acumulamos los resultados en la matriz
    for(int i = 0; i < FREC_FIN-FREC_INI; i++)
    {
        procesado->baselineResultado[i] += resultado[i];
    }

    // Actualizamos el número de segundos de baseline
    procesado->iteraciones++;
}

/**
 * @brief analisisEstimuloAlgoritmo: Llamada cada ventana de análisis para obtener el procesamiento de señal (para el entrenamiento de la interfaz) asociado a cada ventana de tiempo
 * @param procesado: Estructura conetenedora de los datos del análisis
 * @param puntuacion: Puntuación obtenida para una frecuencia dada
 * @param frecuenciaAnalisis: Frecuencia objetivo del análisis
 */
void analisisAlgoritmo(datosAnalisis* procesado){

    // Variables necesarias
    double resultado[FREC_FIN-FREC_INI];

    // Llamamos a la función lanzadora del preprocesado del estímulo
    preprocesado(procesado->vectorAnalisisSolapado, resultado, VENTANA);

    // Almacenamiento de los datos en la matriz y escritura en el archivo de datos
    for (int i = 0; i < FREC_FIN-FREC_INI; i++){
        // Señal final como relación del estímulo respecto del baseline
        if (resultado[i] == 0) procesado->estimuloResultado[i] += 0;
        else if (procesado->baselineResultado[i] == 0) procesado->estimuloResultado[i] += resultado[i];
        else procesado->estimuloResultado[i] += resultado[i]/procesado->baselineResultado[i];
    }

    procesado->iteraciones++;
}

/**
 * @brief puntuacionFrecuenciaAlgoritmo: Función encargada de obtener la puntuacin para la frecuencia objetivo durante la ejecución del algoritmo ACL
 * @param datos: Estructura conetenedora de los datos del análisis
 * @param puntuacion: Valor de la puntuación obtenida para una frecuencia dada
 * @param frecuenciaAnalisis: Frecuencia objetivo del análisis del algoritmo
 */
/*void puntuacionFrecuenciaAlgoritmo(datosAnalisis *datos, double *puntuacion, int *frecuenciaDetectada)
{

    double puntuaciones[MAX_ESTIM] = {0};
    int indiceFrecuenciaMaxima = 0;
    double frecuenciaMaxima = 0;
    int armonico = 0, indiceArmonico = 0;

    // Aplicación del vecino más próximo para deducir la frecuencia observada
    for (int i = 0; i < FREC_FIN-FREC_INI; i++)
    {
        int frecuenciaEvaluar = datos->vectorFrecuencia[i];
        // Buscamos si la frecuencia a evaluar está como frecuencia de estímulo
        for(int j = 0; j < MAX_ESTIM; j++)
        {
            // Si hay coincidencia, analizamos la puntuación para esa frecuencia
            if (frecuenciaEvaluar == datos->frecuenciasAnalisis[j])
            {
                armonico = 2*frecuenciaEvaluar;
                indiceArmonico = 0;
                // Buscamos posición del armónico
                for(int l = 0; l < FREC_FIN-FREC_INI; l++)
                {
                    if (armonico == datos->vectorFrecuencia[l])
                        indiceArmonico = l;
                }
                // Para la puntuación calculamos la puntuación a frecuencia objetivo, valores laterales y primer armónico
                puntuaciones[j] += datos->estimuloResultado[i] + datos->estimuloResultado[i-1]/2 +datos->estimuloResultado[i+1]/2; // Fundamental
                if (indiceArmonico!=0)
                    puntuaciones[j] += datos->estimuloResultado[indiceArmonico] + datos->estimuloResultado[indiceArmonico-1]/2 +datos->estimuloResultado[indiceArmonico+1]/2; // Primer armónico
            }// end if
        } // end for j
    } // end for i

    // Comprobamos cual es el valor máximo
    for (int k = 0;  k < MAX_ESTIM; k++)
    {
      if (puntuaciones[k]>frecuenciaMaxima)
      {
         frecuenciaMaxima=puntuaciones[k];
         indiceFrecuenciaMaxima = k;
      }
    }
    *puntuacion = frecuenciaMaxima;
    *frecuenciaDetectada = datos->frecuenciasAnalisis[indiceFrecuenciaMaxima];
}*/

/**
 * @brief obtenerVectoresAnalisis: Función encargada de operar entre los distintos canales de grabación del casco para analizar las señales deseadas
 * @param canalesDeseados: Array resultadoi de la operación realizada entre los distintos canales, por referencia
 * @param matrizDatos: Matriz con los datos de todos los canales obtenidos del casco
 */
void obtenerVectoresAnalisis(double *canalesDeseados, double **matrizDatos, bestRefers *br)
{
    // Recorremos la matriz de entrada por cada muestra y obtenemos los canales deseados
    for(int i = 0; i < MUESTREO; i++) canalesDeseados[i] = matrizDatos[i][br->firstEle] - matrizDatos[i][br->secondEle];
}

/**
 * @brief obtenerVentanaAnterior: Función encargada de almacenar en la matriz general de datos la ventana anterior para el análisis con solapamientio
 * @param datos: Estructura conetenedora de los datos del análisis
 * @param vectorDatos: Vector con los datos de la ventana anterior
 */
void obtenerVentanaAnterior(datosAnalisis *datos)
{
    // Recorremos las últimas muestras del vector de datos para el solape
    for (int i = 0; i < VENTANA; i++)
        datos->ventanaAnterior[i] = datos->vectorAnalisisSolapado[i];
}

/**
 * @brief obtenerVentanaSolapada: Función encargada de juntar en un mismo array la ventana actual y la parte solapada de la ventana anterior
 * @param datos: Estructura conetenedora de los datos del análisis
 * @param vectorDatos: Vector con los datos de la ventana actual
 */
void obtenerVentanaSolapada(datosAnalisis *datos, double vectorDatos[MUESTREO], int iteracion)
{
    int i, j;
    int temp = VENTANA - DESPL*(iteracion+1);
    // La ventana de análisis estará formada por todas las muestras de la ventana actual y solape con anterior
    for (i = 0; i < temp; i++)
            datos->vectorAnalisisSolapado[i] = datos->ventanaAnterior[i + DESPL*(iteracion+1)];
    for (j = 0; j < DESPL*(iteracion+1); j++){
            datos->vectorAnalisisSolapado[i] = vectorDatos[j];
            i++;
    }
}


// ------------------------------------------FUNCIONES PARA EL ALGORITMO


void distancia(algoritmoDatos *datos){
    int i,j,k,l,n,m;

    double temp, dist = 0.0;

    //se calcula la distancia de la siguiente forma:
    //si tenemos grupos de 2 frecuencias:g[0]=(25,24) g[1]=(35,30)...
    //la distancia entre g[0] y g[1] = (|25-24|+|25-35|+|25-30|+|24-35|+|24-30|+|35-30|)/N de pares posibles, en este caso 6
    for(i = 1; i < datos->numeroGrupos; i++){
        for(j = 0; j < i; j++){
            for(k = 0; k < datos->frecuenciasGrupo; k++)
                for(l = 0; l < datos->frecuenciasGrupo; l++){

                    temp = datos->grupos[i][k] - datos->grupos[j][l];
                    if (temp < 0) temp = -temp;
                    dist += temp;
                }//end of l
            //end of k
            for(n = 1; n < datos->frecuenciasGrupo; n++)
                for(m = 0; m < n; m++){

                    temp = datos->grupos[i][n] - datos->grupos[i][m];
                    if (temp < 0) temp = -temp;
                    dist += temp;

                    temp = datos->grupos[j][n] - datos->grupos[j][m];
                    if (temp < 0) temp = -temp;
                    dist += temp;
                }//end of m
            //end of n
            datos->distancia[i][j] = dist / (datos->frecuenciasGrupo * (datos->frecuenciasGrupo * 2 - 1));
            dist = 0.0;
        }//end of j
    }//end of i
}

void actualizar(algoritmoDatos *datos,int x, int y,double time, double rate){
    datos->puntuacion[x]=datos->puntuacion[x]*(DELTA*rate-GAMMA*time);
    datos->puntuacion[y]=datos->puntuacion[y]*(DELTA*rate-GAMMA*time);
}

void selmaxmat(algoritmoDatos *datos,int *x,int *y){
    int i,j;
    double max = 0.0;
    double compatibility = 0.0;

    for(i=1;i<datos->numeroGrupos;i++)
        for(j=0;j<i;j++){
            compatibility = BETA*datos->distancia[i][j]+ALFA*(datos->puntuacion[i]+datos->puntuacion[j]);
            if(max < compatibility && datos->frecuenciasUtilizadas[i]==0 && datos->frecuenciasUtilizadas[j]==0){
                max = compatibility;
                *x=i;
                *y=j;
            }
        }
}

void cambiar(algoritmoDatos *datos){
    int i,j;
    int x,y;
    algoritmoDatos d1;

    for(i=0; i<datos->numeroGrupos;i++)
        datos->frecuenciasUtilizadas[i] = 0;

    for(i=0;i<datos->numeroGrupos/2;i++){
        selmaxmat(datos,&x,&y);
        datos->frecuenciasUtilizadas[x] = -1;
        datos->frecuenciasUtilizadas[y] = -1;

        d1.puntuacion[i]=datos->numeroGrupos/2-i;
        for(j=0;j<datos->frecuenciasGrupo;j++)
            d1.grupos[i][j]=datos->grupos[x][j];
        for(j=datos->frecuenciasGrupo; j < 2*datos->frecuenciasGrupo; j++)
            d1.grupos[i][j]=datos->grupos[y][j-datos->frecuenciasGrupo];
    }
    datos->numeroGrupos=datos->numeroGrupos/2;
    datos->frecuenciasGrupo=datos->frecuenciasGrupo*2;
    for(i=0;i<datos->numeroGrupos;i++){
        datos->puntuacion[i]=d1.puntuacion[i];
        for(j=0;j<datos->frecuenciasGrupo;j++)
            datos->grupos[i][j]=d1.grupos[i][j];
    }
    distancia(datos);
    // Actualizamos puntuaciones para sets de frecuencias invalidas
    for(i=datos->numeroGrupos;i<FREC_FIN-FREC_INI;i++)
        datos->puntuacion[i]=INT_MIN;
    // Actualizamos frecuencias elegibles
    for(i=0; i<datos->numeroGrupos;i++)
        datos->frecuenciasUtilizadas[i] = 0;

}

/**
 * @brief crearRutaDirectorio: Función encargada de crear la ruta hacia el directorio de cada usuario
 * @param idUsuario: Identificador del usuario
 * @param ruta: Ruta del directorio asociado al usuario
 */
void crearRutaDirectorio( int idUsuario, char* ruta)
{
    // Construcción de rutas y ficheros donde guardaremos información del baseline
    char temporal[LONGITUD_RUTA] = {0};

    // Transformamos el Id de usuario a cadena de caracteres
    char auxInt[3];
    sprintf(auxInt, "%d", idUsuario);

    // Construcción de la ruta del directorio asociado al usuario
    strcpy(temporal, RUTA_USUARIO);
    strcat(temporal, auxInt);
    strcat(temporal, SEPARADOR);
    strcpy(ruta, temporal);
}
