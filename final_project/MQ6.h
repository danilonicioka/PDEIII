#ifndef MQ6_H

#define MQ6_H
#define HEAT_TIME_MQ6                   20000   // Milisseconds

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
double getPPM(double ratio);                // Retorna ppm

int checkRisk(double ppm);      // Retorna nível de risco
                                // SAFE: sem risco
                                // INTOXICATION: risco de intoxicação
                                // EXPLOSION: risco de explosão
                                
int recalibrate(long totalMeasurements); // If it's time to recalibrate, returns 1

#endif
