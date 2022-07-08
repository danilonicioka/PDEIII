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
#include "my_blynk.h"

enum State {
  CALIBRATE,
  CHECK_RISK,
  NOTIFY,
  STANDBY
};

enum Modes {
  NORMAL,
  SET_SAFE,
  SET_INTOXICATION,
  SET_EXPLOSION
};

State state = CALIBRATE;  // Initial state
Modes modes = NORMAL;     // Default mode

DHT dht(DHTPIN, DHTTYPE);

// Sleep configuration
void lightSleep() {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_light_sleep_start();
}


//********************
// BLYNK FUNCTIONS
//********************

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function is called every time the Virtual Pin 1 state changes
BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  switch(value){
    case NO_INFO:{
      modes = NORMAL;
    }
    case SAFE:{
      modes = SET_SAFE;
    }
    case INTOXICATION:{
      modes = SET_INTOXICATION;
    }
    case EXPLOSION:{
      modes = SET_EXPLOSION;
    }
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  
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
      
      if(modes == NORMAL)                 newRisk = checkRisk(ppm);
      else if(modes == SET_SAFE)          newRisk = SAFE;
      else if(modes == SET_INTOXICATION)  newRisk = INTOXICATION;
      else                                newRisk = EXPLOSION;       //modes == SET_EXPLOSION
      
      
      
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
        Blynk.virtualWrite(V0, "Tá safe.");
      }
      else if(newRisk == INTOXICATION){
        Serial.println(F("Entrou em INTOXICATION"));
        Blynk.virtualWrite(V0, "Alerta!");
      }
      else{
        Serial.println(F("Entrou em EXPLOSION"));
        Blynk.virtualWrite(V0, "Corre! O risco de explosão é altíssimo.");
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
