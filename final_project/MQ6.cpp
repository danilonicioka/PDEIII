#define VCC     5
#define RO  53416
#define RL  20000
#define CALIBRATION_SAMPLES                50   // Multisampling
#define CALIBRATION_SAMPLE_INTERVAL        50   // Milisseconds
#define Rs_Ro_clean_air 10.0
#define STD_TEMP        20.0
#define STD_HUM         65.0
#define ADJUST_TEMP     -0.006666667
#define ADJUST_HUM      -0.001923077
#define LOW_RISK         2000
#define MODERATE_RISK   19000
#define RECALIBRATE 360

#include "MQ6.h"
#include "global.h"
#include <math.h>

void setupADC(void){
  // Settings for ADC
  analogSetWidth(10);               // 11Bit resolution
  analogSetAttenuation(ADC_11db);
}

float rawADC(int mq_pin){
  //Multisampling
  uint32_t sum = 0;
  for (uint8_t i = 0; i < CALIBRATION_SAMPLES; i++) {
      sum += analogRead(mq_pin);
      delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  return (float)sum/CALIBRATION_SAMPLES;  
}

float getVoltage(float raw){
  float volt = 2*5*raw/1023.0;  // Multiplica por 2 para compensar o
                                // divisor de tensão do circuito
  if (volt < 0.17) return 0.17;
  else             return volt;
}

float getRs(float voltage){
  return RL*(VCC - voltage)/voltage;
}

float getRo(float Rs){
  return Rs / Rs_Ro_clean_air;
}

void printSamples(int cont, float raw, float volt, float Rs, double ratio, double ppm){
  Serial.print(F("["));
  Serial.print(cont);
  Serial.print(F("]"));
  Serial.print(F("  Raw: "));
  Serial.print(raw);
  Serial.print(F("  Voltage: "));
  Serial.print(volt);
  Serial.print(F("V")); 
  Serial.print(F("  Rs: "));
  Serial.print(Rs);
  Serial.print(F("  Ratio: "));
  Serial.print(Rs/RO);
  Serial.print(F("  Concentration: "));
  Serial.print(ppm);
  Serial.println(F("ppm")); 
}

double getRatio(float Rs){
  return Rs/RO;
}

double adjustRatio(double ratio, float temp, float hum){
  double dTemp = temp - STD_TEMP;
  double dHum  = hum  - STD_HUM;
  double newRatio;
  newRatio = ratio + dTemp*ADJUST_TEMP + dHum*ADJUST_HUM;
//  Serial.println(F("=== Temperature ==="));
//  Serial.printf("Temp: %f\tdTemp: %lf\n\n",temp,dTemp);
//  Serial.println(F("=== Humidity ==="));
//  Serial.printf("Hum: %f\tdHum: %lf\n\n",hum,dHum);
//  Serial.println(F("=== Ratio ==="));
//  Serial.printf("Old ratio: %lf\tNew ratio: %lf\n\n",ratio,newRatio);
  return newRatio;
}

double getPPM(double ratio){
  double expoent = 1.0/log10(0.4);
  double ppm = 1000*pow(ratio, expoent);
//  double newppm = 1000*pow(newRatio, expoent);
//  Serial.println(F("=== Concentration ==="));
//  Serial.printf("Expoent: %lf\tPPM: %lf\tNew PPM: %lf\n\n",expoent,ppm,newppm);
  return ppm;
}

int checkRisk(double ppm){
  if (ppm < LOW_RISK)           return SAFE;
  else if (ppm < MODERATE_RISK) return INTOXICATION;
  else                          return EXPLOSION;
}

int recalibrate(long totalMeasurements){
  if(totalMeasurements % RECALIBRATE == 0)
    return 1;
  return 0;
}
