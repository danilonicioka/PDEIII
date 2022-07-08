#ifndef MY_BLYNK_H
#define MY_BLYNK_H

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLyhEIBMwS"
#define BLYNK_DEVICE_NAME           "Firewall"
#define BLYNK_AUTH_TOKEN            "gkkK2iGtHUfg9R4QKWodaASsSlmc85vq"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Fernandes";
char pass[] = "Fernandes07";

#endif
