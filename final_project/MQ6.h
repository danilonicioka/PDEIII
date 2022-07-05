#ifndef MQ6_H
#define MQ6_H

#include <Arduino.h>

void  setupADC(void);           // Configura resolução (10 bits) e atenuação (11dB) do ADC
float rawADC(int mq_pin);       // Retorna valor do ADC no intervalo [0,1023]
float getVoltage(float raw);       // Retorna tensão na saída analógica do MQ6
float getRs(float voltage);        // Retorna valor da resistência variável Rs
float getRo(float Rs_clean_air);   // Retorna valor de referência Ro, desde que
                                    // receba como parâmetro o valor de Rs no ar limpo
void  printSamples(int cont, float raw, float volt, float Rs, double ratio, double ppm);  // Imprime informações das amostras colhidas

double getRatio(float Rs);         // Retorna ratio Rs/Ro
double adjustRatio(double ratio, float temp, float hum);  // Retorna ratio ajustado de acordo com temp. e hum.
double getPPM(double ratio, double newRatio);                // Retorna ppm

int checkRisk(double ppm);      // Retorna nível de risco
                                // N: sem risco
                                // I: risco de intoxicação
                                // E: risco de explosão

#endif