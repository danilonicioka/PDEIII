#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

//needed for library
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

WiFiManager wifiManager;          //Objeto de manipulação do wi-fi

#define BLYNK_PRINT Serial

//define your default values here, if there are different values in config.json, they are overwritten.
char blynk_token[34] = "your-blynk-token";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
//void saveConfigCallback () {
//  Serial.println("Should save config");
//  shouldSaveConfig = true;
//}

void setup() {
  // put your setup code here, to run once:
//  //callback para quando entra em modo de configuração AP
//  wifiManager.setAPCallback(configModeCallback); 
//  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
//  wifiManager.setSaveConfigCallback(saveConfigCallback); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
