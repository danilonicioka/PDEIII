/*************************************************************
  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
*************************************************************/

//========================================
// GLOBAL SETTINGS
//========================================


// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLyqqttM1j"
#define BLYNK_DEVICE_NAME           "Firewall"
#define BLYNK_AUTH_TOKEN            "dRTjnP4odrTQAiXc1rvcesNnOeF77HSP"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// ADC definitions
#define MQ6 32
#define NO_OF_SAMPLES   64          //Multisampling

//DHT definitions
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Fernandes";
char pass[] = "Fernandes07";

BlynkTimer timer;



uint8_t flag1 = 1;  //Cleaned after notification is sent
uint8_t flag2 = 0;  //Voltage >= 3V



DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

//========================================
// AUXILIARY FUNCTIONS
//========================================

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);

  pinMode(MQ6,INPUT);

  // Initialize DHT.
  dht.begin();
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

uint32_t getRaw(){
  uint32_t adc_reading = 0;
  //Multisampling
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
      adc_reading += analogRead(MQ6);
  }
  adc_reading /= NO_OF_SAMPLES;
  return adc_reading;
}

float getVoltage(uint32_t adc_reading){
  return 3.3*adc_reading/4095.0;
}


void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  Serial.println(F("=== ADC TEST ==="));
}

void loop()
{
  Blynk.run();
  timer.run();
  uint32_t adc_reading = getRaw();
  float    voltage     = getVoltage(adc_reading);

  Serial.print("Raw: ");
  Serial.print(adc_reading);
  Serial.print("\tVoltage: ");
  Serial.println(voltage);

  flag2 = (voltage >= 3) ? 1 : 0;
  
  if(flag1 && flag2){
    Blynk.logEvent("jak_notify");
    flag1=0;
  }
    
  delay(1500);
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
