#define MQ6    35
#define HEAT    1
#define DHTPIN 15
#define DHTTYPE DHT22
#define LIFE_SIGNAL_LIMIT 5
//#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
//#define TIME_TO_SLEEP  5        // Time ESP32 will go to sleep (in seconds)
#define NORMAL            0
#define SET_SAFE          1
#define SET_INTOXICATION  2
#define SET_EXPLOSION     3

#include "MQ6.h"
#include <DHT.h>
#include "global.h"
#include "my_blynk.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;


// State machine data structure
enum State {
  CALIBRATE,
  MEASURE,
  CHECK_RISK,
  NOTIFY,
  STANDBY
};

State state = CALIBRATE;  // Initial state
int checkMode = NORMAL;     // Default mode

DHT dht(DHTPIN, DHTTYPE);

//*******************************
// AUXILIARY FUNCTIONS
//*******************************

//void lightSleep() {
//    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP);
//    esp_light_sleep_start();
//}

void listNetworks(){
  SerialBT.print(F("Buscando redes..."));
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  if (n == 0) {
      SerialBT.println(F("nenhuma rede encontrada\n"));
  } else {
    SerialBT.print(n);
    SerialBT.println(F(" redes encontradas"));
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      SerialBT.print(i);
      SerialBT.print(F(": "));
      SerialBT.print(WiFi.SSID(i));
      SerialBT.print(F(" ("));
      SerialBT.print(WiFi.RSSI(i));
      SerialBT.print(F(")"));
      SerialBT.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
}

int selectNetwork(void){
  SerialBT.print(F("Selecione uma rede -> i: "));
  SerialBT.println(F(""));
  for(uint8_t i=0; ;){
    if(SerialBT.available() > 0) {
      delay(10);
      char c = SerialBT.read();
      int index = c - 48;
      delay(10);
      while(SerialBT.available()) SerialBT.read();
      SerialBT.println(F("Saindo do select"));
      return index;
    }
  }
}

void getCredentials(char credentials[], String title){
  SerialBT.print(F("Inserir "));
  SerialBT.println(title);
  for(uint8_t i=0; ;){
    if(SerialBT.available() > 0) {
      delay(10);
      char c = SerialBT.read();
      if(c=='\n'){
//        SerialBT.println(F("Trying to connect..."));
        SerialBT.println(F("Saindo do getCred"));
        credentials[i-1] = '\0';
        break;
      }
      else {
        credentials[i] = c;
        i++;
      }
    }
  }
}

//*******************************
// BLYNK FUNCTIONS
//*******************************

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
  Serial.println(F("\n=== Pino V1 ==="));
  // Set incoming value from pin V1 to a variable
  int value = param.asInt();
  switch(value){
    case 0:{
      checkMode = NORMAL;
      Serial.println(F("Pino V1 = NORMAL"));
      break;
    }
    case 1:{
      checkMode = SET_SAFE;
      Serial.println(F("Pino V1 = SET_SAFE"));
      break;
    }
    case 2:{
      checkMode = SET_INTOXICATION;
      Serial.println(F("Pino V1 = SET_INTOXICATION"));
      break;
    }
    case 3:{
      checkMode = SET_EXPLOSION;
      Serial.println(F("Pino V1 = Entrou em SET_EXPLOSION"));
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Firewall");
  while(!SerialBT.connected());
  
  listNetworks();
  int index = selectNetwork();
  
  char* ssid;
  String ssid_string = WiFi.SSID(index);
  ssid = &ssid_string[0];
//  Serial.print("Tamanho do SSID: ");
//  Serial.println(strlen(ssid));

  char pass[20];
  char auth[35];
  char ID[25];
  getCredentials(pass, "senha");
  getCredentials(auth, "token");
  getCredentials(ID, "ID");

  String S_ID = ID;
  #define BLYNK_TEMPLATE_ID S_ID
  SerialBT.end();
  
  Serial.print(F("Senha: "));
  Serial.println(pass);
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
  static int contador = 1;
  static long medidas = 1;
  static float Ro;
  
  switch(state){
    case CALIBRATE:{
      Serial.println(F("\n=== ENTERED STATE CALIBRATE ==="));
      float raw           = rawADC(MQ6);
      float volt          = getVoltage(raw);
      float Rs_clean_air  = getRs(volt);
      Ro                  = getRo(Rs_clean_air);
      state = MEASURE;
      break;
    }
    case MEASURE:{
      Serial.println(F("\n=== ENTERED STATE MEASURE ==="));
      float raw = rawADC(MQ6);
      float volt = getVoltage(raw);
      float Rs  = getRs(volt);
      double ratio = getRatio(Rs, Ro);
      ratio = adjustRatio(ratio, dht.readTemperature(), dht.readHumidity());
      ppm = getPPM(ratio);
      printSamples(contador, raw, volt, Rs, ratio, ppm);
      state = CHECK_RISK;
      break;
    }
    
    case CHECK_RISK:{
      Serial.println(F("\n=== ENTERED STATE CHECK_RISK ==="));
      
      if     (checkMode == NORMAL)            newRisk = checkRisk(ppm);
      else if(checkMode == SET_SAFE)          newRisk = SAFE;
      else if(checkMode == SET_INTOXICATION)  newRisk = INTOXICATION;
      else                                    newRisk = EXPLOSION;       //checkMode == SET_EXPLOSION
      
      // If risk has changed, goes to state NOTIFY
      // else, goes to state STANDBY
      state = (newRisk != lastRisk) ? NOTIFY : STANDBY;
      
      lastRisk = newRisk; // Updates lastRisk value
      break;
    }
    
    case NOTIFY:{
      Serial.println(F("\n=== ENTERED STATE NOTIFY ==="));
      if(newRisk == SAFE){
        Serial.println(F("Entrou em SAFE"));
        Blynk.logEvent("safe");
        Blynk.virtualWrite(V0, "Seguro");
      }
      else if(newRisk == INTOXICATION){
        Serial.println(F("Entrou em INTOXICATION"));
        Blynk.logEvent("warning");
        Blynk.virtualWrite(V0, "Perigo");
      }
      else if(newRisk == EXPLOSION){
        Serial.println(F("Entrou em EXPLOSION"));
        Blynk.logEvent("danger");
        Blynk.virtualWrite(V0, "Emergência");
      }

      state = STANDBY;
      break;
    }
    
    case STANDBY:{
      Serial.println(F("\n=== ENTERED STATE STANDBY ==="));
      Serial.println(F("Going to sleep now"));
      delay(5000);
//      lightSleep();
      Serial.println(F("Woke up now"));
      contador++;
      medidas++;
      if(medidas) state = CALIBRATE;
      else        state = MEASURE;
      break;
    }
  }
}
