#define MQ6    35
#define DHTPIN 15
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define HEAT 0

#include "MQ6.h"
#include <DHT.h>
#include "global.h"

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

#if HEAT
  Serial.print(F("MQ6 is heating..."));
  delay(HEAT_TIME_MQ6);
  Serial.println(F("\tdone!\n"));
#endif
  
  pinMode(MQ6,INPUT);
  setupADC();
  dht.begin();
  
  Serial.println(F("=== Reading MQ6 ==="));
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint8_t i = 0;
  while(i<10){
    float raw = rawADC(MQ6);
    float volt = getVoltage(raw);
    float Rs  = getRs(volt);  
    double ratio = getRatio(Rs);
    double newRatio = adjustRatio(ratio, dht.readTemperature(), dht.readHumidity());
    double ppm = getPPM(ratio, newRatio);
    printSamples(i, raw, volt, Rs, newRatio, ppm);
    i++;
  }
}
