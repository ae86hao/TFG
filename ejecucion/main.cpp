//Recibe como parámetros:
// 1 - El número de Usuario
// 2 - El numero de accion a realizar
// 3 - El ID de la cola de mensaje con la interfaz
// 4 - El numero de canales

#include "interfazEEG.h"

/**
 * @brief Orquestador principal de la ejecucuón
 * @return 0 (correcto) o 1 (error)
 */
int main(int argc, char *argv[]){

    analysisAfterFreqScanning(1, 14);

    /*if (argc != 5) return 1;
    int idUsuario = atoi(argv[1]);  // Valor del ID Usuario
    int accion = atoi(argv[2]);  // Valor de la accion
    int idCola = atoi(argv[3]); // Valor de la cola de mensajes
    int numCanales = atoi(argv[4]); // Valor del numero de canales

    switch(accion){
    case 1:
        capturaBaseline(idCola, idUsuario, numCanales);
        break;
    case 2:
        capturaEstimulo(idCola, idUsuario, numCanales);
        break;
    case 3:
        algoritmoFrecuenciasOptimas(idCola, idUsuario, numCanales);
        break;
    case 4:
        barridoFrecuencia(idCola, idUsuario, numCanales);
        break;
    }*/

    return 0;
}
