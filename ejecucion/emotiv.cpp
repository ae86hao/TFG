#include "emotiv.h"
#include "preprocesado.h"

// Lista total de canales a considerar por el EmotivEpoc
EE_DataChannel_t targetChannelList[] = {
        ED_AF3, ED_F7, ED_F3, ED_FC5, ED_T7,
        ED_P7, ED_O1, ED_O2, ED_P8, ED_T8,
        ED_FC6, ED_F4, ED_F8, ED_AF4
};

/**
 * @brief envioDatosCasco: Función encargada de capturar los datos del casco Emotiv Epoc mediante el SDK proporcionado
 * @param idCola: Identificador de la cola a la que se enviarán los datos capturados por el casco
 * @return 0 (correcto) o 1 (error)
 */
void envioDatosCasco(int idCola, int idColaStop, int segundosSimulacion, int numCanales)
{
    // Variables para el control de transmisión de datos
    MensajeIPC mensajeEnviado;
    MensajeIPC mensajeRecibido;
    pid_t pid= getpid();

    // Variables para el control de flujo del EmotivEpoc
    bool capturaDatosLista = false;
    EmoEngineEventHandle eventoEmotiv   = EE_EmoEngineEventCreate();
    EmoStateHandle       estadoEmotiv	= EE_EmoStateCreate();
    int capturaEvento					= 0;
    unsigned int usuarioID				= 0;
    bool signalApagado                  = false;
    int  contadorSegundos               = 0;
    int  contadorMuestras               = 0;

        EE_EngineConnect();

        // Definimos tamaño del buffer y el manipulador del flujo de datos
        DataHandle gestorDatosEEG = EE_DataCreate();
        EE_DataSetBufferSizeInSec(0);

        // Bucle de ejecución principal
        while (signalApagado ==false) {
            //si recibimos el mensaje de stop, terminamos el proceso
            if (msgrcv(idColaStop, &mensajeRecibido, sizeof(mensajeRecibido.texto), 0, MSG_NOERROR | IPC_NOWAIT) > 0){
                if (mensajeRecibido.tipoComunicacion == IPC_STOP){
                    // Liberamos el gestor de flujo de datos
                    EE_DataFree(gestorDatosEEG);

                    // Cerramos las instancias del Emotiv Epoc
                    EE_EngineDisconnect();
                    EE_EmoStateFree(estadoEmotiv);
                    EE_EmoEngineEventFree(eventoEmotiv);
                    kill(pid, SIGTERM);
                }
            }

            // Captura del evento
            capturaEvento= EE_EngineGetNextEvent(eventoEmotiv);

            if (capturaEvento == EDK_OK) // Evento capturado OK
            {
                // Tipo de evento capturado e ID de usuario
                EE_Event_t eventType = EE_EmoEngineEventGetType(eventoEmotiv);
                EE_EmoEngineEventGetUserId(eventoEmotiv, &usuarioID);

                // Si el evento implica una nueva sesión
                if (eventType == EE_UserAdded) {
                    // Habilitamos captura de datos
                    EE_DataAcquisitionEnable(usuarioID,true);
                    capturaDatosLista = true;
                }
            }
            // Si la sesión ya estaba iniciada
            if (capturaDatosLista)
            {
                // Actualización de datos
                EE_DataUpdateHandle(usuarioID, gestorDatosEEG);
                unsigned int muestrasTomadas = 0;

                // Obtenemos el número de la muestra
                EE_DataGetNumberOfSample(gestorDatosEEG,&muestrasTomadas);
                EE_DataSetBufferSizeInSec(0);

                // Si el número de muestras es distinto de 0
                if (muestrasTomadas != 0)
                {
                    // Variable para almacenar los datos de las muestras
                    double** datos = new double*[numCanales];
                    for(int i = 0; i < numCanales; i++)
                        datos[i] = new double[VENTANA];

                    // recorremos todos los canales y obtener los datos
                    for (int indiceCanales = 0 ; indiceCanales < numCanales ; indiceCanales++) {
                        // Capturamos la información de los datos para el canal y la muestra deseadas
                        EE_DataGet(gestorDatosEEG, targetChannelList[indiceCanales], datos[indiceCanales], muestrasTomadas);
                    }

                    // Recorremos todas las muestras y enviarlas de una en una
                    for (int indiceMuestras = 0 ; indiceMuestras<(int)muestrasTomadas ; indiceMuestras++)
                    {
                        mensajeEnviado.tipoComunicacion = IPC_MENSAJE_DATOS; // Mensaje de datos

                        //enviar los datos de la siguiente forma:
                        //datos[c_0][m_0], datos[c_1][m_0], datos[c_2][m_0], ...
                        //datos[c_0][m_1], datos[c_1][m_1], datos[c_2][m_1], ...
                        for (int indiceCanales = 0 ; indiceCanales < numCanales; indiceCanales++) {
                            mensajeEnviado.texto = datos[indiceCanales][indiceMuestras];
                            // Transmisión del mensaje y control de errores
                            msgsnd(idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
                        }
                        // Mensaje de control de fin de línea
                        mensajeEnviado.texto = -1;
                        // Transmisión del mensaje y control de errores
                        msgsnd(idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
                        // Muestra enviada
                        contadorMuestras++;

                        // Si hemos llegado a 128 muestras (1 segundo)
                        if (contadorMuestras == MUESTREO)
                        {
                            contadorSegundos++;
                            contadorMuestras=0;
                        }

                        // Si llegamos al total de segundos de simulación
                        if (segundosSimulacion!=0 && contadorSegundos==segundosSimulacion){
                            signalApagado=true;
                            break;
                        }
                    }//end of for
                }
            }
        }//end of while

        // Liberamos el gestor de flujo de datos
        EE_DataFree(gestorDatosEEG);
    // Escribir mensaje de finalización de transmisión
    mensajeEnviado.tipoComunicacion = IPC_MENSAJE_FIN_TRANSMISION;
    mensajeEnviado.texto = -2;
    msgsnd(idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);

    // Cerramos las instancias del Emotiv Epoc
    EE_EngineDisconnect();
    EE_EmoStateFree(estadoEmotiv);
    EE_EmoEngineEventFree(eventoEmotiv);
    kill(pid, SIGTERM);
}