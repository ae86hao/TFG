/** **************************************
  *
  * Autor: Hao Zhu
  *
  *
  * Interfaz EEG bajo Emotiv Epoc - SSVEPs
  *
  * Proyecto de fin de carrera
  * Ingenieria de informatica
  *
  * Universidad Autónoma de Madrid
  *
  *************************************** */

#include "interfazEEG.h"

/**
 * @brief ejecucion: Funcion encargada de lanzar el proceso gestor de datos del casco y de lanzar,
 *        a su vez, las rutinas encargadas de analizar dichos datos. Generará también la cola que se utilizará
 *        para intercambiar información entre procesos.
 * @param idUsuario: Identificador del usuario (int)
 * @param baseline: Indicador de si hay que grabar baseline (bool).
 * @param algoritmoTest: Indicador de si se trata de una rutina de entrenamiento (bool).
 * @param frecuenciaEstimuloAlgoritmo: Frecuencia a evocar en caso de entrenamiento del sistema.
 * @return 0 (correcto) o 1 (error)
 */
int ejecucion(int idColaTerminal, int idUsuario, bool baseline, bool barridoFreqs, bool selectFreqs, int frecuenciaEstimuloAlgoritmo, int segundosSimulacion, int numCanales)
{
    // Control de procesos
    int pid, error, option;

    // Creamos la cola de mensajes a intercambiar entre los procesos
    int idCola = msgget(IPC_PRIVATE, IPC_PERMISOS|IPC_CREAT|IPC_EXCL);
    if (idCola < 0) {
        return 1;
    }

    int idColaStop = msgget(IPC_PRIVATE, IPC_PERMISOS|IPC_CREAT|IPC_EXCL);
    if (idColaStop < 0) {
        return 1;
    }

    // Crearemos un proceso para controlar el flujo de datos del casco
    switch(pid = fork())
    {
        case 0: // Si se trata del hijo será el proceso creador del flujo de datos
        {
            //el hijo termina con kill dentro de la funcion envioDatosCasco
            envioDatosCasco(idCola, idColaStop, segundosSimulacion, numCanales);

        }
        case -1: // Caso de error en el fork
        {
            break;
        }
        default: // Si se trata del padre, será el proceso analizador de datos
        {
            //option = 1 indica capturar datos para baseline
            if (baseline == true) option = 1;
            //option = 2 indica capturar datos para barrido de frecuencias
            else if (barridoFreqs == true) option = 2;
            //option = 3 indica capturar datos para mejores frecuencias
            else if (selectFreqs == true) option = 3;
            //option = 4 indica captura de estimulos
            else option = 4;

            if(capturaDatosCasco(idCola, idColaTerminal, idColaStop, idUsuario, option, frecuenciaEstimuloAlgoritmo, numCanales)==1) return 1;

            // Rutina de salida para destruir la cola
		    error = msgctl(idCola,IPC_RMID,NULL);
		    if (error < 0) {
		        return 1;
		    }

		    error = msgctl(idColaStop,IPC_RMID,NULL);
		    if (error < 0) {
		        return 1;
		    }

            break;
        }
    }

    
    return 0;
}

/**
 * @brief capturaDatosCasco: Captura de los datos recibidos desde el casco y análisis para su uso
 * @param idCola: Identificador de la cola a la que se enviarán los datos capturados por el casco
 * @param idUsuario: Identificador del usuario al que se le asociará el análisis del baseline
 * @return 0 (correcto) o 1 (error)
 */
int capturaDatosCasco(int idCola, int idColaRobot, int idColaStop, int idUsuario, int option, int freqEstimulo, int numCanales)
{
    // Variables necesarias para el procesamiento y almacenamiento de información
    FILE *g, *h;
    MensajeIPC mensajeRecibido;
    MensajeIPC mensajeEnviado;
    EnvioMensajeRobot mensajeRobot;

    int numeroMuestras = 0, numeroCanales = 0;
    int indiceVentana = 0;
    double **matrizMuestras = new double*[MUESTREO];
    for(int i = 0; i < MUESTREO; i++)
        matrizMuestras[i] = new double[numCanales];
    double vectorCanales[MUESTREO];
    double lecturaBaseline;
    int lecturaConfig;
    int frecuenciasFinales[MAX_ESTIM] = {0};
    int status;
    int success_counter = 0;
    int failure_limit = MAX_DURACION;
    int frequency_pre = -1;

    // Matriz de datos general para procesamiento
    datosAnalisis procesoDatos;
    // Inicialización
    procesoDatos.iteraciones = 0;
    for(int i = 0; i < FREC_FIN - FREC_INI; i++){
        procesoDatos.baselineResultado[i] = 0.0;
        procesoDatos.estimuloResultado[i] = 0.0;
    }

    bestRefers br[FREC_FIN-FREC_INI];
    bestRefers *brFound;

    bestRefers brTemp;
    brTemp.freq = freqEstimulo;

    // Construcción de rutas y ficheros donde guardaremos información del baseline
    char rutaBaseline[LONGITUD_RUTA] = {0};
    char rutaSelectFreqs[LONGITUD_RUTA] = {0};
    char rutaConfiguracion[LONGITUD_RUTA] = {0};
    char rutaBrutosBaseline[LONGITUD_RUTA] = {0};
    char rutaBrutosFreq[LONGITUD_RUTA] = {0};
    char rutaSelectRefs[LONGITUD_RUTA] = {0};
    
    //si option == 1, baseline
    if (option == 1){
        crearRutaDirectorio(idUsuario, rutaBrutosBaseline);
        strcat(rutaBrutosBaseline, RUTA_BRUTOS_BASELINE);

        // Apertura del fichero de escritura de baseline brutos
        g = fopen(rutaBrutosBaseline, "w");
    }
    //si no, barrido de frecuencias
    else if (option == 2){
        // Apertura del fichero de escritura de frecuencias brutos
        crearRutaDirectorio(idUsuario, rutaBrutosFreq);
        strcat(rutaBrutosFreq, RUTA_BRUTOS_BARRIDO);
        char auxInt[3];
        sprintf(auxInt, "%d", freqEstimulo);
        strcat(rutaBrutosFreq, auxInt);

        g = fopen(rutaBrutosFreq, "w");
    }
    else{
        crearRutaDirectorio(idUsuario, rutaBaseline);
        strcat(rutaBaseline, RUTA_BASELINE);

        // Apertura del fichero de lectura de baseline
        g = fopen(rutaBaseline,"r");
        for (int i = 0; i < FREC_FIN-FREC_INI; i++)
        {
            fscanf(g,"%lf;", &lecturaBaseline);
            procesoDatos.baselineResultado[i] = lecturaBaseline;
        }
        fclose(g);

        crearRutaDirectorio(idUsuario, rutaSelectRefs);
        strcat(rutaSelectRefs, RUTA_SELECCION_REF);

        // find the best electrodes for the freqStim
        g = fopen(rutaSelectRefs, "r");

        for (int i = 0; i < FREC_FIN-FREC_INI; i++)
            fscanf(g, "electrodos: %d %d, con snr: %lf a %d Hz\n", &br[i].firstEle, &br[i].secondEle, &br[i].snr, &br[i].freq);

        fclose(g);

        brFound = find(br, br+(FREC_FIN-FREC_INI), brTemp);
        
        if (option == 3){
            crearRutaDirectorio(idUsuario, rutaSelectFreqs);

            // Apertura del fichero de escritura de seleccion de frecuencias
            strcat(rutaSelectFreqs, RUTA_SELECT_FREQS);
            g = fopen(rutaSelectFreqs,"a");
        }
        else{
            crearRutaDirectorio(idUsuario, rutaConfiguracion);

            strcat(rutaConfiguracion, RUTA_CONFIGURACION);
            g = fopen(rutaConfiguracion,"r");
            for (int i = 0; i < MAX_ESTIM; i++)
            {
                fscanf(g, "%d ", &lecturaConfig);
                frecuenciasFinales[i] = lecturaConfig;
            }
            fclose(g);
        }
    }

    int duracionSegundos = 0;

    // Mantenemos el proceso escuchando la cola
    while(1)
    {
        // Lectura del mensaje de la cola
        msgrcv(idCola, &mensajeRecibido, sizeof(mensajeRecibido.texto), 0, MSG_NOERROR);

        // Analizamos la información según el tipo de dato almacenado en la cola
        switch(mensajeRecibido.tipoComunicacion){
            case IPC_MENSAJE_FIN_TRANSMISION:   // Finalizamos la transmisión
            {
                // Esperamos a que el proceso hijo finalice
                wait(&status);

                fclose(g);
                return 0;
            }

            case IPC_MENSAJE_DATOS: // Mensaje típico de datos
            {
                // Si recibimos mensaje de fin de línea comprobamos si hay que procesar
                if (mensajeRecibido.texto== -1){
                    // Actualizamos los contadores
                    numeroMuestras++;
                    numeroCanales=0;
                    if (option == 1 || option == 2)  fprintf(g,"\n");

                    // Si llegamos a tener un ciclo de muestreo(128 muestras)
                    if (numeroMuestras == MUESTREO)
                    {
                        numeroMuestras=0;

                        if (option == 3 || option == 4){
                            // Si no tenemos aun una ventana completa(2s), vamos a ir rellenandola
                            if (duracionSegundos < TIMEWINDOW)
                            {
                                // Operamos entre los canales deseados
                                obtenerVectoresAnalisis(vectorCanales, matrizMuestras, brFound);
                                // Almacenamos la información en el array de solape
                                for (int i = 0; i < MUESTREO; i++, indiceVentana++)
                                    procesoDatos.vectorAnalisisSolapado[indiceVentana] = vectorCanales[i];

                                // Si es la primera ventana completa, la analizamos
                                if (duracionSegundos == TIMEWINDOW - 1){
                                    obtenerVentanaAnterior(&procesoDatos);
                                    analisisAlgoritmo(&procesoDatos);

                                    if (option == 3){
                                        //Resultado[0] = amplitud de la frecuencia FREC_INI_REAL
                                        if (procesoDatos.estimuloResultado[freqEstimulo-FREC_INI_REAL] >= UMBRAL_DETEC){
                                            success_counter++;
                                            //cada vez que supera, aumentamos el tiempo maximo de scaneo
                                            failure_limit++;
                                        }
                                        else failure_limit--;

                                        //se resetean los valores para cada iteracion
                                        for(int i = 0; i < FREC_FIN - FREC_INI; i++) procesoDatos.estimuloResultado[i] = 0.0;
                                    }
                                    //si == 4, captura de estimulos
                                    else if (option == 4){
                                        h = fopen("/home/gnb/Hao_Zhu/ejecucion/datos/capDC_log_option4", "a");
                                        double maximo = 0.0;
                                        int frequency = 0;
                                        int temp = 0;
                                        for(int i = 0; i < MAX_ESTIM; i++){
                                            temp = frecuenciasFinales[i];
                                            if (maximo < procesoDatos.estimuloResultado[temp-FREC_INI_REAL]){
                                                maximo = procesoDatos.estimuloResultado[temp-FREC_INI_REAL];
                                                frequency = temp;
                                            }
                                        }
                                        if (maximo >= UMBRAL_DETEC){
                                            frequency_pre = frequency;
                                            success_counter++;
                                        }
                                        fprintf(h, "PRIMERA VENTANA, option 4, freq %d, maximo: %lf\n", frequency, maximo);
                                        fprintf(h, "success_counter: %d\n", success_counter);
                                        //se resetean los valores para cada iteracion
                                        for(int i = 0; i < FREC_FIN - FREC_INI; i++) procesoDatos.estimuloResultado[i] = 0.0;
                                        fclose(h);
                                    }
                                }
                                duracionSegundos++;
                            }
                            // Si ya teníamos alguna ventana para hacer solape
                            else
                            {
                                // Operamos entre los canales deseados
                                obtenerVectoresAnalisis(vectorCanales, matrizMuestras, brFound);
                                //ya tenemos al menos una ventana de 2s, desplazamos cada 250ms hasta llegar al final del nuevo muestreo
                                //en total son 4 desplazamientos (250ms * 4 = 1s)
                                for (int i = 0; i < 4; i++){
                                    // Solapamos ventana actual con parte de la anterior
                                    obtenerVentanaSolapada(&procesoDatos, vectorCanales, i);
                                    //antes de que la ultima ventana sea analizada(al analizar modifica los valores), la conservamos como ventana anterior
                                    if (i == 3) obtenerVentanaAnterior(&procesoDatos);

                                    // Análisis de la señal
                                    analisisAlgoritmo(&procesoDatos);
                                    //si == 3, seleccion de frecuencias
                                    if (option == 3){
                                        //Resultado[0] = amplitud de la frecuencia FREC_INI_REAL
                                        if (procesoDatos.estimuloResultado[freqEstimulo-FREC_INI_REAL] >= UMBRAL_DETEC){
                                            success_counter++;
                                            failure_limit++;

                                            if (success_counter == DETECCION){
                                                fprintf(g, "F. detectada: %d, Tiempo detección: %lf\n", freqEstimulo, procesoDatos.iteraciones * 0.25);//0.25s es el desplazamiento
                                                // Enviamos mensaje para parar el proceso hijo
                                                mensajeEnviado.tipoComunicacion = IPC_STOP;
                                                mensajeEnviado.texto = 999.0;
                                                msgsnd(idColaStop, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
                                                // Esperamos a que el proceso hijo finalice
                                                wait(&status);
                                                fclose(g);
                                                return 0;
                                            }
                                        }
                                        else{
                                            failure_limit--;
                                            success_counter = 0;
                                            //si alcanza el numero maximo de intentos(4s/0.25s=16intentos)
                                            if(failure_limit == 0){
                                                // Enviamos mensaje para parar el proceso hijo
                                                mensajeEnviado.tipoComunicacion = IPC_STOP;
                                                mensajeEnviado.texto = 999.0;
                                                msgsnd(idColaStop, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
                                                // Esperamos a que el proceso hijo finalice
                                                wait(&status);
                                                fclose(g);
                                                return 0;
                                            }
                                        }
                                        //se resetean los valores para cada iteracion
                                        for(int i = 0; i < FREC_FIN - FREC_INI; i++) procesoDatos.estimuloResultado[i] = 0.0;
                                    }
                                    //si == 4, captura de estimulos
                                    else if (option == 4){
                                        h = fopen("/home/gnb/Hao_Zhu/ejecucion/datos/capDC_log_option4", "a");
                                        double maximo = 0.0;
                                        int frequency = 0;
                                        int temp = 0;
                                        for(int i = 0; i < MAX_ESTIM; i++){
                                            temp = frecuenciasFinales[i];
                                            if (maximo < procesoDatos.estimuloResultado[temp-FREC_INI_REAL]){
                                                maximo = procesoDatos.estimuloResultado[temp-FREC_INI_REAL];
                                                frequency = temp;
                                            }
                                        }
                                        if (maximo >= UMBRAL_DETEC){
                                            //si la frecuencia es igual que la anterior, acumulamos el contador de exito
                                            if (frequency_pre == frequency){
                                                success_counter++;
                                                if (success_counter == DETECCION){
                                                    success_counter = 0;
                                                    fprintf(h, "F. detectada: %d, Tiempo detección: %lf\n", frequency, procesoDatos.iteraciones * 0.25);//0.25s es el desplazamiento*/

                                                    // Enviamos mensaje para la GUI
                                                    mensajeRobot.tipoComunicacion = IPC_ROBOT_DATOS;
                                                    mensajeRobot.ComandoRobot[0] = frequency;
                                                    msgsnd(idColaRobot, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
                                                }
                                            }
                                            else{
                                                frequency_pre = frequency;
                                                success_counter = 1;
                                            }
                                        }
                                        fprintf(h, "option 4, freq %d, maximo: %lf\n", frequency, maximo);
                                        fprintf(h, "success_counter: %d\n", success_counter);
                                        //se resetean los valores para cada iteracion
                                        for(int i = 0; i < FREC_FIN - FREC_INI; i++) procesoDatos.estimuloResultado[i] = 0.0;
                                        fclose(h);
                                    }//end of option
                                }//end of for
                            }
                        }
                    }
                }
                // Si recibimos mensaje normal, almacenamos información
                else{
                    // Captura del caracter de entrada
                    double dato = mensajeRecibido.texto;
                    matrizMuestras[numeroMuestras][numeroCanales] = dato;
                    if (option == 1 || option == 2) fprintf(g, "%lf;", dato);
                    numeroCanales++;
                }
                
                break;
            }
        }// Fin del swith
    }// Fin del while
}

/**
 * @brief algoritmoFrecuenciasOptimas: Función encargada de realizar la búsqueda de frecuencias de estimulación óptimas para cada usuario
 * @param idUsuario: Identificador del usuario
 * @return 0 (correcto) o 1 (error)
 */
int algoritmoFrecuenciasOptimas(int idColaTerminal, int idUsuario, int numCanales)
{
    FILE *f, *g;
    char rutaBaseline[LONGITUD_RUTA] = {0};
    char rutaBarridoFreqs[LONGITUD_RUTA] = {0};
    char rutaSelectFreqs[LONGITUD_RUTA] = {0};
    char rutaConfiguracion[LONGITUD_RUTA] = {0};
    struct stat descriptorFichero;
    EnvioMensajeRobot mensajeRobot;
    int errorTransmisionRobot;
    int frecAnalisis = 0;
    int frecuenciasFinales[MAX_ESTIM] = {-1};
    int control;

    puntuacionesAlgoritmo puntuaciones[FREC_FIN-FREC_INI];
    algoritmoDatos resultadosACL;

    g = fopen("/home/gnb/Hao_Zhu/ejecucion/datos/selecFre_log", "a");

    // Inicializamos las variables
    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
            puntuaciones[i].frecuencia = 0;
            puntuaciones[i].puntuacion = 0;
    }
    for (int i = 0; i < FREC_FIN-FREC_INI; i++)
    {
        resultadosACL.numeroGrupos = 0;
        resultadosACL.puntuacion[i] = 0;
        for (int j = 0; j <FREC_FIN-FREC_INI; j++)
            resultadosACL.distancia[i][j] = 0;
        for (int j = 0; j < MAX_ESTIM; j++)
            resultadosACL.grupos[i][j]=-1;
    }

    // Creación de las rutas de los archivos
    crearRutaDirectorio(idUsuario, rutaBaseline);
    crearRutaDirectorio(idUsuario, rutaBarridoFreqs);
    crearRutaDirectorio(idUsuario, rutaSelectFreqs);
    crearRutaDirectorio(idUsuario, rutaConfiguracion);
    strcat(rutaBaseline, RUTA_BASELINE);
    strcat(rutaBarridoFreqs, RUTA_BARRIDO_FREQS);
    strcat(rutaSelectFreqs, RUTA_SELECT_FREQS);
    strcat(rutaConfiguracion, RUTA_CONFIGURACION);

    // Comprobamos si existe el fichero de baseline
    int resultado1 = stat(rutaBaseline, &descriptorFichero);
    if (resultado1 != 0){
        fprintf(g, "No existe fichero de baseline asociado al usuario\n");
        fclose(g);
        return 1;
    }


    /* Barrido de frecuencias
     * ---------------------------- */

    // Información al usuario y confirmación
    fprintf(g, "Se va a proceder a la primera parte del algoritmo: Comprobación singular de todas las frecuencias.\n");



    // Obtención de los datos del análisis
    f = fopen(rutaBarridoFreqs, "r");
    int superaUmbral=0;

    for(int i = 0; i < FREC_FIN-FREC_INI; i++)
    {
        puntuaciones[i].frecuencia = FREC_INI + i;
        fscanf(f, "%lf;", &puntuaciones[i].puntuacion);
        // Analizamos que frecuencias son aptas si superan el umbral y el tiempo mínimo de detección
        if (puntuaciones[i].puntuacion > UMBRAL)
                superaUmbral++;
    }
    fclose(f);
    fprintf(g, "SuperaUmbral: %d\n", superaUmbral);
    // Comprobamos que no existan frecuencias múltiplos de otras frecuencias; Nos quedamos con la que tenga mayor puntuación en ese caso
    /*for (int i = 0; i < FREC_FIN-FREC_INI; i++)
    {
        // Si la frecuencia ha superado el umbral
        if (puntuaciones[i].puntuacion > UMBRAL)
        {
            // Cálculo del armónico
            int frecuenciaArmonico = puntuaciones[i].frecuencia *2;
            int indiceArmonico = -1;
            // Búsqueda del armónico
            for (int j = 0; j < FREC_FIN - FREC_INI; j++)
                 if (puntuaciones[j].frecuencia == frecuenciaArmonico) indiceArmonico = j;
            // Si el armónico estaba por encima del umbral, actualizamos
            if (puntuaciones[indiceArmonico].puntuacion > UMBRAL && indiceArmonico != -1) superaUmbral--;
            // Comprobamos cual es el mayor de los dos y anulamos el valor del otro. En caso de empate nos quedamos con la frecuencia más baja
            if (puntuaciones[i].puntuacion > puntuaciones[indiceArmonico].puntuacion && indiceArmonico != -1) puntuaciones[indiceArmonico].puntuacion = 0;
            if (puntuaciones[i].puntuacion < puntuaciones[indiceArmonico].puntuacion && indiceArmonico != -1) puntuaciones[i].puntuacion = 0;
            if (puntuaciones[i].puntuacion == puntuaciones[indiceArmonico].puntuacion && i < indiceArmonico && indiceArmonico != -1) puntuaciones[indiceArmonico].puntuacion = 0;
            if (puntuaciones[i].puntuacion == puntuaciones[indiceArmonico].puntuacion && i > indiceArmonico && indiceArmonico != -1) puntuaciones[i].puntuacion = 0;
        }
    }*/

    // Ordenamos las frecuencias de mayor a menor
    qsort(puntuaciones, FREC_FIN-FREC_INI, sizeof(puntuacionesAlgoritmo), ordenar);

    for(int i = 0; i < FREC_FIN-FREC_INI; i++)
        fprintf(g, "frecuencia: %d, puntuacion: %lf\n",puntuaciones[i].frecuencia, puntuaciones[i].puntuacion );

    // Actualizamos datos para la primera fase
    resultadosACL.frecuenciasGrupo = 1;
    resultadosACL.numeroGrupos = superaUmbral;

    fprintf(g, "Resultados de la primera fase de mejor a peor:\n");

    // Determinamos los grupos y puntuaciones que han pasado el umbral (grupos de 1 frecuencia)
    for(int i = 0; i < superaUmbral; i++)
    {
        resultadosACL.grupos[i][0] = puntuaciones[i].frecuencia;
        resultadosACL.puntuacion[i]= superaUmbral - i; // Valor N-i
        fprintf(g, "Frecuencia (Hz): %d\t\t", resultadosACL.grupos[i][0]);
        fprintf(g, "Puntuacion     : %lf\n", resultadosACL.puntuacion[i]);
    }

    // Si solo han superado el umbral 4 frecuencias no es necesario seguir con el algoritmo
    if (superaUmbral == 4)
    {
        for (int i = 0; i < superaUmbral; i++)
            frecuenciasFinales[i] = puntuaciones[i].frecuencia;

        // Para las frecuencias elegidas creamos el fichero de configuración asociado al usuario
        f = fopen(rutaConfiguracion,"w+");
        for(int i = 0; i < MAX_ESTIM; i++)
                fprintf(f, "%d ", frecuenciasFinales[i]);

        mensajeRobot.tipoComunicacion = IPC_ROBOT_SELECT_FIN;
        errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
        if (errorTransmisionRobot < 0)
        {
            fprintf(g, "Transmisión de datos fallida\n");
            return 1;
        }

        fclose(f);
        fclose(g);
        return 0;
    }

    // Comprobamos si hay las suficientes frecuencias viables
    if (superaUmbral < 4)
    {
        fprintf(g, "No se ha podido obtener el número mínimo de frecuencias óptimas\n");
        fclose(g);
        return 1;
    }

    // Calculamos la distancia entre las frecuencias aptas
    distancia(&resultadosACL);


    /* ALGORITMO ACL
     * ---------------------------- */

    int x = 0,y = 0;

    // Iteramos hasta conseguir las 4 frecuencias óptimas
    // Iteracion 1(fase 1) = con grupos de 2 frecuencias
    // Iteracion 2(fase 2) = con grupos de 4 frecuencias
    for (int iteracion = 0; iteracion < 2; iteracion++)
    {
      fprintf(g, "iteracion %d\n", iteracion);
        int duracion;
        int secuencia[MAX_DURACION + 1];//+1 para tener una posicion mas que indica fin de secuencia(con valor -1)
        switch(iteracion)
        {
        // Apertura del archivo de estimulación en función de cuantas frecuencias estemos utilizando
        case 0: f=fopen(RUTA_SECUENCIA_2,"r"); break;
        case 1: f=fopen(RUTA_SECUENCIA_4,"r"); break;
        }

        // Número de cambios de objetivo a los que mirar en secuencia de entrenamiento
        fscanf(f, "%d\n", &duracion);
        // Almacenamos la secuencia hacia donde tendrá que mirar el usuario
        for(int j = 0; j < duracion; j++)
            fscanf(f, "%d ", &secuencia[j]);
        secuencia[duracion] = -1; // Indicamos que se termina la secuencia
        fclose(f);

        // Inicializamos la información de frecuencias utilizadas (-1; frecuencias no útiles)
        for (int e = 0; e < FREC_FIN-FREC_INI; e++)
            resultadosACL.frecuenciasUtilizadas[e] = -1;
        // Inicializamos las frecuencias utilizables
        for (int e = 0; e < resultadosACL.numeroGrupos; e++)
            resultadosACL.frecuenciasUtilizadas[e] = 0;

        // Número de iteraciones 3N/4
        for (int j = 0; j < resultadosACL.numeroGrupos * 3/4; j++)
       {
            int interf[MAX_ESTIM] = {0};
            double tiempoDeteccion = 0.0;
            double tasaAciertos = 0.0;

            // Obtención máxima compatbilidad de frecuencias no utilizadas
            int valorAnteriorX = x;
            int valorAnteriorY = y;
            selmaxmat(&resultadosACL,&x,&y);

            // Si no se eligen nuevos valores de x e y, reiniciamos
            if (x==valorAnteriorX && y== valorAnteriorY)
            {
                for (int e = 0; e < resultadosACL.numeroGrupos; e++)
                    resultadosACL.frecuenciasUtilizadas[e] = 0;
                selmaxmat(&resultadosACL,&x,&y);
            }
            // Selección de frecuencias a representar en esta iteración
            for(int k=0;k<resultadosACL.frecuenciasGrupo;k++){
                interf[k]=resultadosACL.grupos[x][k]; // Guardamos la frecuencia del grupo maxmat
                resultadosACL.frecuenciasUtilizadas[x] = 1; // Marcamos la frecuencia como utilizada
            }
            for(int k=0;k<resultadosACL.frecuenciasGrupo;k++){
                interf[k+resultadosACL.frecuenciasGrupo]=resultadosACL.grupos[y][k];
                resultadosACL.frecuenciasUtilizadas[y] = 1; // Marcamos la frecuencia como utilizada
            }

            // Parte de estimulación gráfica y análisis de datos
            int posFrecAnalisis;

            // Configuración de todos los estímulos
            mensajeRobot.tipoComunicacion = IPC_ROBOT_CONFIG;
            for (int k = 0; k < MAX_ESTIM; k++)
                mensajeRobot.ComandoRobot[k] = -1;

            switch(iteracion)
            {
            case 0:
                mensajeRobot.ComandoRobot[0] = interf[0];
                mensajeRobot.ComandoRobot[1] = interf[1];
                break;
            case 1:
                mensajeRobot.ComandoRobot[0] = interf[0];
                mensajeRobot.ComandoRobot[1] = interf[1];
                mensajeRobot.ComandoRobot[2] = interf[2];
                mensajeRobot.ComandoRobot[3] = interf[3];
                break;
            }

            // mandamos la frecuencia a estimular hacia la interfaz
            errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
            if (errorTransmisionRobot < 0)
            {
                fprintf(g, "Transmisión de datos fallida\n");
                fclose(g);
                return 1;
            }

            for (int e = 0; e < duracion; e++)
            {
                // Posición de la frecuencia a estimular
                posFrecAnalisis = secuencia[e];

                // Transformamos la posición de la frecuencia a estimular a la frecuencia real del estímulo a observar
                frecAnalisis = interf[posFrecAnalisis];

                // mandamos la posicion del estimulo hacia donde el usuario debe mirar
                mensajeRobot.tipoComunicacion = IPC_ROBOT_POS;
                mensajeRobot.ComandoRobot[0] = posFrecAnalisis + STIMPOS;
                errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
                if (errorTransmisionRobot < 0)
                {
                    fprintf(g, "Transmisión de datos fallida\n");
                    fclose(g);
                    return 1;
                }

                sleep(2); // Esperamos a que la interfaz esté iniciada

                
                control = ejecucion(idColaTerminal, idUsuario, false, false, true, frecAnalisis, SIMTIME_ACL, numCanales);
                if (control==1)
                {
                    fprintf(g, "Fallo al intentar grabar el estímulo\n");
                    fclose(g);
                    return 1;
                }

                mensajeRobot.tipoComunicacion = IPC_ROBOT_POS_FIN;
                errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
                if (errorTransmisionRobot < 0)
                {
                    fprintf(g, "Transmisión de datos fallida\n");
                    fclose(g);
                    return 1;
                }

                sleep(3);

            }//end of e

            mensajeRobot.tipoComunicacion = IPC_ROBOT_CONFIG_FIN;
            errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
            if (errorTransmisionRobot < 0)
            {
                fprintf(g, "Transmisión de datos fallida\n");
                fclose(g);
                return 1;
            }

            /* Análisis de datos para la iteración
             * ---------------------------- */

           f = fopen(rutaSelectFreqs, "r");
           int frecuenciaDetectada=0;
           double tiempo = 0.0;
           double sucess = 0.0;

           while(!feof(f))
           {
               // Cargamos datos desde el fichero
                fscanf(f, "F. detectada: %d, Tiempo detección: %lf\n", &frecuenciaDetectada, &tiempo);
                fprintf(g, "F. detectada: %d, Tiempo detección: %lf\n", frecuenciaDetectada, tiempo);
                sucess++;
                tiempoDeteccion += tiempo;
           }

           //borrar el fichero
           fclose(f);
           if(remove(rutaSelectFreqs) != 0){
              fprintf(g, "fallo al intentar eliminar fichero SelectFreqs\n");
              fclose(g);
              return 1;
           }

           // tasa de aciertos
           tasaAciertos = sucess/duracion;

           fprintf(g, "Tiempo de detección correcta: %lf\n", tiempoDeteccion);
           fprintf(g, "Tasa de detecciones correctas: %lf\n", tasaAciertos);

           // Descanso de tiempo para el usuario
           mensajeRobot.tipoComunicacion = IPC_ROBOT_COMM;
           mensajeRobot.ComandoRobot[0] = j+1;
           mensajeRobot.ComandoRobot[1] = resultadosACL.numeroGrupos * 3/4;
           errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
           if (errorTransmisionRobot < 0)
           {
               fprintf(g, "Transmisión de datos fallida\n");
               fclose(g);
               return 1;
           }
           sleep(7);

           actualizar(&resultadosACL,x,y,tiempoDeteccion,tasaAciertos);
           for(int w = 0; w < resultadosACL.numeroGrupos; w++)
               for(int v = 0; v < resultadosACL.frecuenciasGrupo; v++)
                fprintf(g, "grupo %d, freq %d con puntuacion: %lf\n", w, resultadosACL.grupos[w][v], resultadosACL.puntuacion[w]);
        }
        //Fin del bucle 3N/4


        // Actualizamos información y cambiamos la disposición de los grupos para 4 frecuencias
        cambiar(&resultadosACL);

        // Si después de la actualización solo nos quedan 2 pares de frecuencias no es necesario seguir
        if (resultadosACL.numeroGrupos <= 2 && resultadosACL.frecuenciasGrupo == 2){
            fprintf(g, "Solo queda una única combinación de 4 frecuencias disponible\n");
            break;
        }
    }

    // Primeramente no consideraremos frecuencias que no hayan formado grupos de 4
    for (int i = 0; i < FREC_FIN - FREC_INI; i++)
        if (resultadosACL.grupos[i][0] == -1) resultadosACL.puntuacion[i] = 0;

    // Obtenemos las 4 frecuencias mayores
    int valorTemporal = 0;
    int maxi = 0;

    for (int j = 0; j < resultadosACL.numeroGrupos; j++)
    {
        if (valorTemporal < resultadosACL.puntuacion[j])
        {
            valorTemporal = resultadosACL.puntuacion[j];
            maxi = j;
        }
    }

    for (int j = 0; j < resultadosACL.frecuenciasGrupo; j++)
        frecuenciasFinales[j] = resultadosACL.grupos[maxi][j];

    fprintf(g, "Frecuencias finales elegidas por puntuacion\n");
    f = fopen(rutaConfiguracion,"w+");

    for (int i = 0; i < MAX_ESTIM; i++){
        fprintf(g, "%dHz\n", frecuenciasFinales[i]);
        fprintf(f, "%d ", frecuenciasFinales[i]);
    }

/*    // Información de cuál sería el resultado si en el último paso seleccionamos máxima compatibilidad en vez de máxima puntuación
    int valorAnteriorX = x;
    int valorAnteriorY = y;
    int resultCom[FREC_FIN - FREC_INI];
    selmaxmat(&resultadosACL,&x,&y);
    // Si no se eligen nuevos valores de x e y, reiniciamos
    if (x==valorAnteriorX && y== valorAnteriorY)
    {
        for (int e = 0; e < resultadosACL.numeroGrupos; e++)
            resultadosACL.frecuenciasUtilizadas[e] = 0;
        selmaxmat(&resultadosACL,&x,&y);
    }
    // Selección de frecuencias a representar en esta iteración
    for(int k=0;k<resultadosACL.frecuenciasGrupo;k++){
        resultCom[k]=resultadosACL.grupos[x][k];
    }
    for(int k=0;k<resultadosACL.frecuenciasGrupo;k++){
        resultCom[k+resultadosACL.frecuenciasGrupo]=resultadosACL.grupos[y][k];
    }

    fprintf(g, "Frecuencias finales elegidas por compatibilidad\n");
    for (int k = 0; k < FREC_FIN - FREC_INI; k++ )
            fprintf(g, "%d\n", resultCom[k]);
*/
    // Para las frecuencias elegidas creamos el fichero de configuración asociado al usuario

    //mensaje de finalizar
    mensajeRobot.tipoComunicacion = IPC_ROBOT_SELECT_FIN;
    errorTransmisionRobot = msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR);
    if (errorTransmisionRobot < 0)
    {
        fprintf(g, "Transmisión de datos fallida\n");
        return 1;
    }

    fclose(f);
    fclose(g);
    return 0;
}



/**
 * @brief capturaBasline: Función encarga de realizar la captura de baseline
 * @param idColaTerminal: cola de mensajes
 * @param idUsuario: id del usuario
 * @return 0 en caso de exito, 1 en caso de error
 */
int capturaBaseline(int idColaTerminal, int idUsuario, int numCanales){
    EnvioMensajeRobot mensajeRobot;
    int control;

    sleep(1);

    control = ejecucion(idColaTerminal, idUsuario, true, false, false, 0, SIMTIME_BASELINE, numCanales);
    if (control==1) {return 1;}

    system("beep");

    mensajeRobot.tipoComunicacion = IPC_ROBOT_BASE_FIN;
    if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) {return 1;}

    return 0;
}

/**
 * @brief barridoFrecuencia: Función encarga de realizar el primer barrido de frecuencias
 * @param idColaTerminal: cola de mensajes
 * @param idUsuario: id del usuario
 * @return 0 en caso de exito, 1 en caso de error
 */
int barridoFrecuencia(int idColaTerminal, int idUsuario, int numCanales){
    EnvioMensajeRobot mensajeRobot;
    int control;
    int frecAnalisis = 0;

    // Primera fase: Ejecución para cada frecuencia del rango válido
    for(int i = 0; i < FREC_FIN-FREC_INI; i ++)
    {
        //configurar las frecuencias a -1 indica luces apagadas
        for (int j = 0; j < MAX_ESTIM; j++)
            mensajeRobot.ComandoRobot[j] = -1;

        // Frecuencia a analizar
        frecAnalisis = FREC_INI + i;

        // mandamos la frecuencia a estimular hacia la interfaz
        mensajeRobot.tipoComunicacion = IPC_ROBOT_CONFIG;
        mensajeRobot.ComandoRobot[0] = frecAnalisis;
        if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) {return 1;}

        sleep(2); // Esperamos a que la interfaz esté iniciada

        
        control = ejecucion(idColaTerminal, idUsuario, false, true, false, frecAnalisis, SIMTIME_FREQ, numCanales);
        if (control==1) {return 1;}

        // Apagar los LEDs
        mensajeRobot.tipoComunicacion = IPC_ROBOT_CONFIG_FIN;
        if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) {return 1;}

        // Fase de descanso 2s
        mensajeRobot.tipoComunicacion = IPC_ROBOT_COMM;
        mensajeRobot.ComandoRobot[0] = i+1;
        mensajeRobot.ComandoRobot[1] = FREC_FIN - FREC_INI;
        if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) {return 1;}

        sleep(7);
    }

    analysisAfterFreqScanning(idUsuario, numCanales);

    mensajeRobot.tipoComunicacion = IPC_ROBOT_BARRIDO_FIN;
    if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) {return 1;}

    return 0;
}

/**
 * @brief analysisAfterFreqScanning: analyze data from file after baseline and freq scanning
 * @param idUsuario: id del usuario
 * @return 0 en caso de exito, 1 en caso de error
 */
int analysisAfterFreqScanning(int idUsuario, int numCanales){
    datosAnalisis procesoDatos;
    double** baseline;
    FILE *f, *g;
    bestRefers br[FREC_FIN-FREC_INI];
    bestRefers *brFound;
    bestRefers brTemp;

    procesoDatos.iteraciones = 0;
    for(int i = 0; i < FREC_FIN - FREC_INI; i++){
        procesoDatos.baselineResultado[i] = 0.0;
        procesoDatos.estimuloResultado[i] = 0.0;
    }

    char rutaBrutosBaseline[LONGITUD_RUTA] = {0};
    char rutaSelectRefs[LONGITUD_RUTA] = {0};
    
    crearRutaDirectorio(idUsuario, rutaBrutosBaseline);
    strcat(rutaBrutosBaseline, RUTA_BRUTOS_BASELINE);

    //------------- best reference algorithm -----------------

    //      ------- baseline part -------

    f = fopen(rutaBrutosBaseline, "r");
    baseline = read_file_BCI(f, SIMTIME_BASELINE, numCanales); //leer grabacion de baseline
    fclose(f);

    //      ------- frequency part -------

    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
        char rutaBrutosFreq[LONGITUD_RUTA] = {0};
        char auxInt[3];

        crearRutaDirectorio(idUsuario, rutaBrutosFreq);
        strcat(rutaBrutosFreq, RUTA_BRUTOS_BARRIDO);
        sprintf(auxInt, "%d", i+FREC_INI);
        strcat(rutaBrutosFreq, auxInt);

        f = fopen(rutaBrutosFreq, "r");

        selectReferencia(idUsuario, f, baseline, i+FREC_INI, numCanales);
        fclose(f);
    }

    // get the best pair of references for each frequency
    crearRutaDirectorio(idUsuario, rutaSelectRefs);
    strcat(rutaSelectRefs, RUTA_SELECCION_REF);

    f = fopen(rutaSelectRefs, "r");

    for (int i = 0; i < FREC_FIN-FREC_INI; i++)
        fscanf(f, "electrodos: %d %d, con snr: %lf a %d Hz\n", &br[i].firstEle, &br[i].secondEle, &br[i].snr, &br[i].freq);

    fclose(f);

    //------------- SNR calculation -----------------


    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
        char rutaBrutosFreq[LONGITUD_RUTA] = {0};
        char auxInt[3];

        // find the best electrodes for the frequency
        brTemp.freq = i+FREC_INI;
        brFound = find(br, br+(FREC_FIN-FREC_INI), brTemp);

        //      ------- baseline part -------

        f = fopen(rutaBrutosBaseline, "r");
        calcSNRBaselineFromFiles(idUsuario, f, &procesoDatos, brFound, numCanales);


        //      ------- frequency part -------

        crearRutaDirectorio(idUsuario, rutaBrutosFreq);
        strcat(rutaBrutosFreq, RUTA_BRUTOS_BARRIDO);
        
        sprintf(auxInt, "%d", i+FREC_INI);
        strcat(rutaBrutosFreq, auxInt);

        g = fopen(rutaBrutosFreq, "r");

        calcSNRFreqFromFiles(idUsuario, g, i+FREC_INI, &procesoDatos, brFound, numCanales);

        fclose(f);
        fclose(g);
    }

    return 0;
}

/**
 * @brief capturaEstimulo: Función encarga de realizar la captura de estimulos
 * @param idColaTerminal: cola de mensajes
 * @param idUsuario: id del usuario
 * @return 0 en caso de exito, 1 en caso de error
 */
int capturaEstimulo(int idColaTerminal, int idUsuario, int numCanales){
    EnvioMensajeRobot mensajeRobot;
    int control;

    sleep(1); // Esperamos a que la interfaz esté iniciada

    
    control = ejecucion(idColaTerminal, idUsuario, false, false, false, 0, SIMTIME_REAL, numCanales);
    if (control==1) return 1;

    // Apagar los LEDs
    mensajeRobot.tipoComunicacion = IPC_ROBOT_CONFIG_FIN;
    if (msgsnd(idColaTerminal, &mensajeRobot, sizeof(mensajeRobot), MSG_NOERROR) < 0) return 1;

    return 0;
}


/**
 * @brief selectReferencia: Función encarga de seleccionar las mejores referencias
 * @param
 * @return 0 en caso de exito, 1 en caso de error
 */
int selectReferencia(int idUsuario, FILE *freq, double **basedata, int freqStim, int numCanales){
    
    double** signal;
    int signal_size = 0;

    char rutaSelectRefs[LONGITUD_RUTA] = {0};

    crearRutaDirectorio(idUsuario, rutaSelectRefs);
    strcat(rutaSelectRefs, RUTA_SELECCION_REF);
    
    signal = read_file_BCI(freq, SIMTIME_FREQ, numCanales); //leer grabacion de senales

    best_referencies(signal, SIMTIME_FREQ*MUESTREO, basedata, SIMTIME_BASELINE*MUESTREO, VENTANA, freqStim, rutaSelectRefs, numCanales);
    
    for(int i = 0; i < numCanales; i++) free(signal[i]);
    
    return 0;
}

/**
 * @briefint calcSNRBaselineFromFiles(int idUsuario, FILE *f, datosAnalisis *procesoDatos, bestRefers *brFound); calcSNRBaselineFromFiles: function that get SNR from files of Baseline
 * @param
 * @return 0 en caso de exito, 1 en caso de error
 */
int calcSNRBaselineFromFiles(int idUsuario, FILE *baseline, datosAnalisis *procesoDatos, bestRefers *brFound, int numCanales){
    double **datos = new double*[MUESTREO];
    for(int i = 0; i < MUESTREO; i++)
        datos[i] = new double[numCanales];
    double vectorCanales[MUESTREO];
    FILE *f;
    char * pch;
    char caracteres[1000];
    int numMuestras, duracionSegundos, indiceVentana;

    char rutaBaseline[LONGITUD_RUTA] = {0};    

    crearRutaDirectorio(idUsuario, rutaBaseline);
    strcat(rutaBaseline, RUTA_BASELINE);

    //------------------------ calculate SNR ------------------------
    duracionSegundos = 0;
    indiceVentana = 0;

    for (numMuestras = 0; !feof(baseline); numMuestras++){
        fgets(caracteres, 1000, baseline);
        pch = strtok (caracteres,";");

        for(int channel = 0; pch != NULL && channel < numCanales; channel++){
            datos[numMuestras][channel] = atof(pch);
            pch = strtok (NULL, ";");
        }

        if (numMuestras == MUESTREO - 1){
            numMuestras = 0;

            if (duracionSegundos < TIMEWINDOW){
                // get the desired channels's data
                obtenerVectoresAnalisis(vectorCanales, datos, brFound);
                // store the information in the overlap array
                for (int i = 0; i < MUESTREO; i++, indiceVentana++)
                        procesoDatos->vectorAnalisisSolapado[indiceVentana] = vectorCanales[i];

                // if it is the 1st window, we analyze it
                if (duracionSegundos == TIMEWINDOW - 1){
                    obtenerVentanaAnterior(procesoDatos);
                    analisisBaseline(procesoDatos);
                }
                duracionSegundos++;
            }
            else{
                obtenerVectoresAnalisis(vectorCanales, datos, brFound);
                //if we are here means that we already have at least 1 window, so with the new token of data now we displace every 250ms until the end
                //in total there are 4 displacements (250ms * 4 = 1s)
                for (int i = 0; i < 4; i++){
                    // overlap the current window with part of the previous
                    obtenerVentanaSolapada(procesoDatos, vectorCanales, i);
                    //before the last window to be analyzed, we store it
                    if (i == 3) obtenerVentanaAnterior(procesoDatos);
                    analisisBaseline(procesoDatos);
                }
            }
        }//end of if(numMuestras)
    }//end of for(!feof)

    // write the results into the baseline file for further analysis
    f = fopen(rutaBaseline,"w");

    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
        procesoDatos->baselineResultado[i] = procesoDatos->baselineResultado[i] / procesoDatos->iteraciones;
        fprintf(f, "%lf;", procesoDatos->baselineResultado[i]);
    }
    
    fclose(f);

    return 0;
}

/**
 * @brief calcSNRFreqFromFiles: function that get SNR from files of Frequency
 * @param
 * @return 0 en caso de exito, 1 en caso de error
 */
int calcSNRFreqFromFiles(int idUsuario, FILE *freq, int freqStim, datosAnalisis *procesoDatos, bestRefers *brFound, int numCanales){
    double **datos = new double*[MUESTREO];
    for(int i = 0; i < MUESTREO; i++)
        datos[i] = new double[numCanales];
    double vectorCanales[MUESTREO];
    FILE *f;
    char * pch;
    char caracteres[1000];
    int numMuestras, duracionSegundos, indiceVentana;
        
    char rutaBarridoFreqs[LONGITUD_RUTA] = {0};

    crearRutaDirectorio(idUsuario, rutaBarridoFreqs);
    strcat(rutaBarridoFreqs, RUTA_BARRIDO_FREQS);
    

    //------------------------ calculate SNR ------------------------

    duracionSegundos = 0;
    indiceVentana = 0;
    procesoDatos->iteraciones = 0;

    for (numMuestras = 0; !feof(freq); numMuestras++){
        fgets(caracteres, 1000, freq);
        pch = strtok (caracteres,";");

        for(int channel = 0; pch != NULL && channel < numCanales; channel++){
            datos[numMuestras][channel] = atof(pch);
            pch = strtok (NULL, ";");
        }

        if (numMuestras == MUESTREO - 1){
            numMuestras = 0;

            if (duracionSegundos < TIMEWINDOW){
                // get the desired channels's data
                obtenerVectoresAnalisis(vectorCanales, datos, brFound);
                // store the information in the overlap array
                for (int i = 0; i < MUESTREO; i++, indiceVentana++)
                        procesoDatos->vectorAnalisisSolapado[indiceVentana] = vectorCanales[i];

                // if it is the 1st window, we analyze it
                if (duracionSegundos == TIMEWINDOW - 1){
                    obtenerVentanaAnterior(procesoDatos);
                    analisisAlgoritmo(procesoDatos);
                }
                duracionSegundos++;
            }
            else{
                obtenerVectoresAnalisis(vectorCanales, datos, brFound);
                //if we are here means that we already have at least 1 window, so with the new token of data now we displace every 250ms until the end
                //in total there are 4 displacements (250ms * 4 = 1s)
                for (int i = 0; i < 4; i++){
                    // overlap the current window with part of the previous
                    obtenerVentanaSolapada(procesoDatos, vectorCanales, i);
                    //before the last window to be analyzed, we store it
                    if (i == 3) obtenerVentanaAnterior(procesoDatos);
                    analisisAlgoritmo(procesoDatos);
                }
            }
        }//end of if(numMuestras)
    }//end of for(!feof)

    // write the results into the baseline file for further analysis
    f = fopen(rutaBarridoFreqs,"w");

    for(int i = 0; i < FREC_FIN-FREC_INI; i++){
        procesoDatos->estimuloResultado[i] = procesoDatos->estimuloResultado[i] / procesoDatos->iteraciones;
        fprintf(f, "%lf;", procesoDatos->estimuloResultado[i]);
    }
    
    fclose(f);
    
    return 0;
}

/**
 * @brief ordenar: Función lanzadora para ordenar las fercuencias en función de la puntuación obtenida
 * @param a: Primera estructura a comparar
 * @param b: Segunda estructura a comparar
 * @return Llamada a la función comparar con las estructuras
 */
int ordenar(const void* a, const void* b)
{
    puntuacionesAlgoritmo *p1 = (puntuacionesAlgoritmo*)a;
    puntuacionesAlgoritmo *p2 = (puntuacionesAlgoritmo*)b;

    return comparar(p1->puntuacion, p2->puntuacion);
}

/**
 * @brief comparar: Función encargada de comparar dos valores de tipo double
 * @param a: Primer parámetro a comparar
 * @param b: Segundo paráemtro a comprarar
 * @return 0 (parámetros iguales), 1 (segundo parámetro mayor que primero), -1 (primer parámetro mayor que el segundo)
 */
int comparar (double a, double b)
{
    if (a > b) return -1;
    if (a == b) return 0;
    else return 1;
}


