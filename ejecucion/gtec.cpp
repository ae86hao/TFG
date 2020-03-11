#include "gtec.h"
#include "preprocesado.h"

int flag = 0;

void CallBackMaster( void* parameter )
{
    int capSize;
    float dataFloat[5000];
    double dataDouble[5000];
    unsigned char usr_buffer_master[ 10000 ];
    int numMuestras;
    MensajeIPC mensajeEnviado;
    MensajeIPC mensajeRecibido;
    parameters* pa = (parameters*)(parameter);
    FILE *f;

    if (pa->apagado == 0){
        //if we receive the stop message, finish the processs
        if (msgrcv(pa->idColaStop, &mensajeRecibido, sizeof(mensajeRecibido.texto), 0, MSG_NOERROR | IPC_NOWAIT) > 0){
            if (mensajeRecibido.tipoComunicacion == IPC_STOP){
                flag = 1;
                return;
            }
        }
        size_t cnt_master = GT_GetSamplesAvailable( MASTER );
        GT_GetData( MASTER, usr_buffer_master, cnt_master);

        capSize=static_cast<int>(cnt_master);

        memcpy(dataFloat,usr_buffer_master,capSize);
        //size of float: 4 bytes, if we have 256 datas in char, then 64 in float
        capSize = capSize/4;

        //cast to double because it is the common type we used for further analysis
        for(int i = 0; i < capSize; i++) dataDouble[i] = static_cast<double>(dataFloat[i]);

        //take into account the number of channels: if we have 64 datas, then 4 samples(each contains 16 channel's info)
        numMuestras = capSize / 16;
        int index = 0;

        for (int indiceMuestras = 0 ; indiceMuestras < numMuestras ; indiceMuestras++)
        {
            mensajeEnviado.tipoComunicacion = IPC_MENSAJE_DATOS; // Mensaje de datos

            for (int counter = 0 ; counter < pa->numCanales; counter++) {
                mensajeEnviado.texto = dataDouble[index];
                // Transmisión del mensaje y control de errores
                msgsnd(pa->idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
                index++;
            }
            // Mensaje de control de fin de línea
            mensajeEnviado.texto = -1;
            // Transmisión del mensaje y control de errores
            msgsnd(pa->idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);
            // Muestra enviada
            pa->sampleCounter++;
            // Si hemos llegado a MUESTREO muestras (1 segundo)
            if (pa->sampleCounter == MUESTREO)
            {
                pa->secondCounter++;
                pa->sampleCounter = 0;
            }

            // Si llegamos al total de segundos de simulación
            if (pa->secondCounter == pa->segundosSimulacion){
                pa->apagado = 1;

                mensajeEnviado.tipoComunicacion = IPC_MENSAJE_FIN_TRANSMISION;
                mensajeEnviado.texto = -2;
                msgsnd(pa->idCola, &mensajeEnviado, sizeof(mensajeEnviado.texto), MSG_NOERROR);

                flag = 1;
                return;
            }
        }//end of for(indiceMuestras)
    }
}


/**
 * @brief envioDatosCasco;: Función encargada de capturar los datos del casco G-Tec mediante el SDK proporcionado
 * @param idCola: Identificador de la cola a la que se enviarán los datos capturados por el casco
 * @return 0 (correcto) o 1 (error)
 */
void envioDatosCasco(int idCola, int idColaStop, int segundosSimulacion, int numCanales)
{
    pid_t pid= getpid();
	const int sample_rate = MUESTREO;
	gt_usbamp_analog_out_config ao_config_master;
	ao_config_master.shape = GT_ANALOGOUT_SINE;
	ao_config_master.frequency = 10;
	ao_config_master.amplitude = 200;
	ao_config_master.offset = 0;

	gt_usbamp_config config_master;
	config_master.ao_config = &ao_config_master;
	config_master.sample_rate = sample_rate;
	config_master.number_of_scans = GT_NOS_AUTOSET;
	config_master.enable_trigger_line = GT_FALSE;
	config_master.scan_dio = GT_FALSE;
	config_master.slave_mode = GT_FALSE;
	config_master.enable_sc = GT_FALSE;
	config_master.mode = GT_MODE_NORMAL;
	config_master.num_analog_in = numCanales;

	for ( unsigned int i = 0; i < GT_USBAMP_NUM_GROUND; i++ )
	{
	config_master.common_ground[i] = GT_TRUE;
	config_master.common_reference[i] = GT_TRUE;
	}

	for ( unsigned char i = 0; i < config_master.num_analog_in ; i++ )
	{
	config_master.analog_in_channel[i] = i + 1;
	config_master.bandpass[ i ] = GT_FILTER_NONE;
	config_master.notch[ i ] =  GT_FILTER_NONE;
	config_master.bipolar[ i ] = GT_BIPOLAR_DERIVATION_NONE;
	}

	GT_UpdateDevices();
	GT_OpenDevice( MASTER );

	GT_SetConfiguration( MASTER, &config_master );

	parameters parameter;
	parameter.idCola = idCola;
	parameter.idColaStop = idColaStop;
	parameter.sampleCounter = 0;
	parameter.secondCounter = 0;
    parameter.apagado = 0;
    parameter.numCanales = numCanales;
	parameter.segundosSimulacion = segundosSimulacion;
	GT_SetDataReadyCallBack( MASTER, &CallBackMaster, (void*)(&parameter) );
	GT_StartAcquisition( MASTER );

	//the process will finish by callBack function, so set this sleeping long time enough
	while(1) if(flag == 1) break;
    GT_StopAcquisition( MASTER );
    GT_CloseDevice( MASTER );
    kill(pid, SIGTERM);
}

