/** **************************************
  *
  * Autor: Álvaro Morán García
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


#ifndef EE_EMOTIV_H
#define EE_EMOTIV_H

#include "best_referencies.h"
#include "emotiv.h"
#include "gtec.h"

int ejecucion(int idColaTerminal, int idUsuario, bool baseline, bool barridoFreqs, bool selectFreqs, int frecuenciaEstimuloAlgoritmo, int segundosSimulacion, int numCanales);
int capturaDatosCasco(int idCola, int idColaRobot, int idColaStop, int idUsuario, int option, int freqEstimulo, int numCanales);
int ordenar(const void *a, const void *b);
int comparar (double a, double b);
int algoritmoFrecuenciasOptimas(int idColaTerminal, int idUsuario, int numCanales);
int capturaBaseline(int idColaTerminal, int idUsuario, int numCanales);
int barridoFrecuencia(int idColaTerminal, int idUsuario, int numCanales);
int analysisAfterFreqScanning(int idUsuario, int numCanales);
int selectReferencia(int idUsuario, FILE *freq, double **basedata, int freqStim, int numCanales);
int capturaEstimulo(int idColaTerminal, int idUsuario, int numCanales);
int calcSNRBaselineFromFiles(int idUsuario, FILE *baseline, datosAnalisis *procesoDatos, bestRefers *brFound, int numCanales);
int calcSNRFreqFromFiles(int idUsuario, FILE *freq, int freqStim, datosAnalisis *procesoDatos, bestRefers *brFound, int numCanales);

#endif // EE_EMOTIV_H
