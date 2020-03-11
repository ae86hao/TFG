
#include "best_referencies.h"

using namespace std;

double** read_file_BCI(FILE* f, int simtime, int numCanales){

    double** datos;
    datos = new double*[numCanales];

    int channel,fila;
    char * pch;
    char caracteres[1000];

    for(channel=0; channel < numCanales; channel++) datos[channel] = new double[simtime*MUESTREO];
    
    //continuos reallocs para los proximos datos
    for(fila=0; !feof(f) && fila < simtime*MUESTREO ;fila++)
    {
        fgets(caracteres, 1000, f);
        pch = strtok (caracteres,";");

        for(channel=0;pch != NULL && channel < numCanales;channel++){
            datos[channel][fila]=atof(pch);
            pch = strtok (NULL, ";");
        }
    }

    return datos;
}


int number_of_windows(int signal_size, int window_size){
    return (signal_size-window_size)/DESPL+1;
}

double* difference_two_signal(double* first, double* second, int size ){

    double* resul = NULL;

    if (size <= 0) return NULL;
    else
    {
        resul = (double*)malloc(size * sizeof(double));
        for(int i = 0; i < size; i++){
            resul[i] = first[i] - second[i];
            if (resul[i] < 0) resul[i] = -resul[i];
        }
    }

    return resul;
}



double* frequency_study(int size_window, double* signal, int nwindows){

    int ventana, inicio;
    double* resul;
    double temp;
    double auxiliar[size_window];

    resul = (double*)malloc((FREC_FIN-FREC_INI) * sizeof(double));

    for(int i=0; i < FREC_FIN-FREC_INI; i++) resul[i]=0.0;

    for(ventana=1, inicio=0; ventana <= nwindows; ventana++){
		
            for(int i=0; i < size_window; i++){
                auxiliar[i] = signal[inicio+i];
			}
            inicio = ventana * DESPL;
			
            detrend(auxiliar, size_window);
            enventanadoHanning(auxiliar, size_window);
            gsl_fft_real_radix2_transform(auxiliar, 1, size_window);

            for(int i = 0; i < FREC_FIN-FREC_INI; i++){
                // Access to the real and imaginary values of each frequency and calculate the amplitude
                temp = REAL(auxiliar,(i+FREC_INI)*2)*REAL(auxiliar,(i+FREC_INI)*2)+IMAG(auxiliar,(i+FREC_INI)*2)*IMAG(auxiliar,(i+FREC_INI)*2);
                //resutaldo[0] = amplitud de la frecuencia FREC_INI, respectivamente
                resul[i] += sqrt(temp);
            }
	}
		
    for(int i = 0; i < FREC_FIN-FREC_INI; i++) resul[i] = resul[i] / nwindows;

    return resul;
}


void best_referencies(double** signal, int signal_size, double** baseline, int baseline_size, int nsamples_window, int freqStim, char* ruta, int numCanales){

    int nwindows_baseline, nwindows_signal;
    double* difference_signal;
    double* difference_baseline;
    double* resul_signal;
    double* resul_baseline;
    double **snr = new double*[numCanales];
    for(int i = 0; i < numCanales; i++)
        snr[i] = new double[numCanales];
    double max = 0.0;
    int index_1 = 0;
    int index_2 = 0;
    FILE *f;

    f = fopen(ruta, "a");
    nwindows_baseline = number_of_windows(baseline_size,nsamples_window);
    nwindows_signal = number_of_windows(signal_size,nsamples_window);

    for(int i=0; i < numCanales; i++)
        for(int j=i+1; j<numCanales; j++){ //no queremos repetir numeros ni la resta de dos numeros iguales
            difference_signal = difference_two_signal(signal[i], signal[j], signal_size);
            difference_baseline = difference_two_signal(baseline[i], baseline[j], baseline_size);

            resul_signal = frequency_study(nsamples_window, difference_signal, nwindows_signal);
            resul_baseline = frequency_study(nsamples_window, difference_baseline, nwindows_baseline);

            snr[i][j] = resul_signal[freqStim-FREC_INI] / resul_baseline[freqStim-FREC_INI];

            free(difference_baseline);
            free(difference_signal);
            free(resul_signal);
            free(resul_baseline);
        }

    for(int i=0; i < numCanales; i++)        
        for(int j=i+1; j<numCanales; j++)
            if (max < snr[i][j]){
                max = snr[i][j];
                index_1 = i;
                index_2 = j;
            }

    fprintf(f, "electrodos: %d %d, con snr: %lf a %d Hz\n", index_1, index_2, max, freqStim);
    fclose(f);
}








