
#include "preprocesado.h"

#define ARMONICO 1 // 0 si no quieres tener en cuenta los armónicos

double** read_file_BCI(FILE* f, int simtime, int numCanales);
int number_of_windows(int signal_size, int window_size);
double* difference_two_signal(double* first, double* second, int size );
double* frequency_study(int size_window, double* signal, int nwindows);
void best_referencies(double** signal, int signal_size, double** baseline, int baseline_size, int nsamples_window, int freqStim, char* ruta, int numCanales);



