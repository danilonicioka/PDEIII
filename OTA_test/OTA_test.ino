/*
    This example combines wifimanager and blynk, and saves wifi login and blynk token
    through resets and power cycles.
    How it works:
    Wifimanager attempts to connect to default network. If unable, it opens an Access
    Point which you can connect to using a device with wifi and a browser.
    Simply put in the new network name, password, and the token given in your blynk app,
    then connect your device to the same new wifi.
    If the connection is successful, it will store the connection information and
    attempt to reconnect after resets!

    Make sure to read through all the code.
    You'll have to download a few libraries, choose a name and password for the AP,
    and uncomment wifiManager.resetSettings(); the first time you upload. For final code,
    ensure it's commented out.
    Good luck!
    */
    
    ------------------------------------------------------------------------------------------

    #include <FS.h>                   //this needs to be first, or it all crashes and burns...


    #include "ESP32_WiFiManager.h"          //https://github.com/tzapu/WiFiManager
    #include <WiFi.h>
    #include <BlynkSimpleEsp32.h>
    
    //needed for library
    #include <DNSServer.h>
    #include <WebServer.h>

    #include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

    #define BLYNK_PRINT Serial
//
//    #define BLYNK_TEMPLATE_ID "TMPLyqqttM1j"
//    #define BLYNK_DEVICE_NAME "Quickstart Template"
//    #define BLYNK_AUTH_TOKEN "dRTjnP4odrTQAiXc1rvcesNnOeF77HSP"
    //define your default values here, if there are different values in config.json, they are overwritten.
    char blynk_token[34] = "your-blynk-token";

    //flag for saving data
    bool shouldSaveConfig = false;

    //callback notifying us of the need to save config
    void saveConfigCallback () {
      Serial.println("Should save config");
      shouldSaveConfig = true;
    }
 
    ------------------------------------------------------------------------------------------

  void setup() {
      // put your setup code here, to run once:
      Serial.begin(115200);
      Serial.println();

      //clean FS, for testing
      //SPIFFS.format();

      //read configuration from FS json
      Serial.println("mounting FS...");

      if (SPIFFS.begin()) {
        Serial.println("mounted file system");
        if (SPIFFS.exists("/config.json")) {
          //file exists, reading and loading
          Serial.println("reading config file");
          File configFile = SPIFFS.open("/config.json", "r");
          if (configFile) {
            Serial.println("opened config file");
            size_t size = configFile.size();
            // Allocate a buffer to store contents of the file.
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);
            //Json Document initialize
            DynamicJsonDocument doc(256);   //adjust the size of the Json Document to your needs by changing (256)
            auto error = deserializeJson(doc, buf.get());
            serializeJson(doc, Serial);

            if (!error) {
              Serial.println("\nparsed doc");
              strcpy(blynk_token, doc["blynk_token"]);
    
            } else {
              Serial.print(F("deserializeJson() failed with code "));
              Serial.println(error.c_str());
            }
          }
        }
      } else {
        Serial.println("failed to mount FS");
      }
      //end read


      // The extra parameters to be configured (can be either global or just in the setup)
      // After connecting, parameter.getValue() will get you the configured value
      // id/name placeholder/prompt default length
      WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 34);

      //Local intialization. Once its business is done, there is no need to keep it around
      WiFiManager wifiManager;

      wifiManager.setSaveConfigCallback(saveConfigCallback);

      //add all your parameters here
      wifiManager.addParameter(&custom_blynk_token);

      
      //reset settings - for testing - it's a good idea to run this one time, then comment it out for final code
      wifiManager.resetSettings();


      //set minimum quality of signal so it ignores AP's under that quality
      //defaults to 8%
      //wifiManager.setMinimumSignalQuality();
  
      //fetches ssid and pass and tries to connect
      //if it does not connect it starts an access point with the specified name
      //and goes into a blocking loop awaiting configuration
      if (!wifiManager.autoConnect("Firewall", "password")) {     //Choose a name and password for the AP to be created
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }

      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");

      //read updated parameters
      strcpy(blynk_token, custom_blynk_token.getValue());
    
      //save the custom parameters to FS
      if (shouldSaveConfig) {
        Serial.println("saving config");
        DynamicJsonDocument doc(256);    //adjust the size of the Json Document to your needs by increasing (256)
        doc["blynk_token"] = blynk_token;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
          Serial.println("failed to open config file for writing");
        }

        serializeJson(doc, Serial);
        serializeJson(doc, configFile);
        configFile.close();
        //end save
      }

      //configure blynk
      delay(5000);
      Blynk.config(blynk_token);
      bool result = Blynk.connect(180);
      if (result != true) {
        Serial.println("BLYNK Connection Fail");
        //needs testing on whether this line should be commented out:
        //wifiManager.resetSettings();
        ESP.reset();
        delay (5000);
      }
      else  {
        Serial.println("BLYNK Connected");
      }
      
      //Connect to blynk using an already-open internet connection and preset configuration
      if(!Blynk.connect()) {
        Serial.println("Blynk connection timed out.");
        ESP.reset();
        delay (5000);
      }
    }

    ------------------------------------------------------------------------------------------

  void loop() {
      // put your main code here, to run repeatedly:
      Blynk.run();
    }
