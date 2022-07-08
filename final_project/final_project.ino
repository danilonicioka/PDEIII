#define MQ6    35
#define HEAT    0
#define DHTPIN 15
#define DHTTYPE DHT22
#define LIFE_SIGNAL_LIMIT 5
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  5        // Time ESP32 will go to sleep (in seconds)

#include "MQ6.h"
#include <DHT.h>
#include "global.h"

enum State {
  CALIBRATE,
  CHECK_RISK,
  NOTIFY,
  STANDBY
};

State state = CALIBRATE;  // Initial state

DHT dht(DHTPIN, DHTTYPE);

// Sleep configuration
void lightSleep() {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_light_sleep_start();
}

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

  static double ppm;
  static int lastRisk = NO_INFO;
  static int newRisk = NO_INFO;
  static int lifeSignal = 0;
  static int contador = 0;
  
  switch(state){
    case CALIBRATE:{
      Serial.println(F("=== ENTERED STATE CALIBRATE ==="));
      float raw = rawADC(MQ6);
      float volt = getVoltage(raw);
      float Rs  = getRs(volt);
      double ratio = getRatio(Rs);
      ratio = adjustRatio(ratio, dht.readTemperature(), dht.readHumidity());
      ppm = getPPM(ratio);
      printSamples(contador, raw, volt, Rs, ratio, ppm);
      state = CHECK_RISK;
      break;
    }
    
    case CHECK_RISK:{
      Serial.println(F("=== ENTERED STATE CHECK_RISK ==="));
      newRisk = checkRisk(ppm);
      
      // If risk has changed, goes to state NOTIFY
      // else, goes to state STANDBY
      if(newRisk != lastRisk){
        lifeSignal = 0;
        state = NOTIFY;
      }
      else{
        lifeSignal++;
        if(lifeSignal == LIFE_SIGNAL_LIMIT){
          lifeSignal = 0;
          state = NOTIFY;
        } else state = STANDBY;
      }
      
      lastRisk = newRisk; // Updates lastRisk value
      break;
    }
    
    case NOTIFY:{
      Serial.println(F("=== ENTERED STATE NOTIFY ==="));
      if(newRisk == SAFE){
        Serial.println(F("Entrou em SAFE"));
      }
      else if(newRisk == INTOXICATION){
        Serial.println(F("Entrou em INTOXICATION"));
      }
      else{
        Serial.println(F("Entrou em EXPLOSION"));
      }

      state = STANDBY;
      break;
    }
    
    case STANDBY:{
      Serial.println(F("=== ENTERED STATE STANDBY ==="));
      Serial.println(F("Going to sleep now"));
      delay(1000);
      lightSleep();
      Serial.println(F("Woke up now"));
      contador++;
      state = CALIBRATE;
      break;
    }
  }
}
