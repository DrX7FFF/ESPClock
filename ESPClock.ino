// OTA : generic module / 80 Mhz / QIO / 40 MHz / 512k (0kb) / 230400
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "Afficheur.h"
#include "HTTPTime.h"

#include "PrivateWifi.h"
//const char* ssid     = "....";
//const char* password = "....";
const char* host = "ESP_CLOCK";

byte seconde;
byte myTicks;
byte initialized;
// Use WiFiClient class to create TCP connections

// Passer en seconde
void myeventtick(){
  myTicks++;
  if (myTicks == 5) {
    myTicks = 0;
    seconde++;
    if (seconde == 60) {
      seconde = 0;
      minute++;
      if (minute == 60) {
        minute = 0;
        hour++;
        if (hour == 24) {
          hour = 0;
        }
      }
      MatrixShow();
    }
  }
  if (seconde & 1)
    MatrixIntensity(5 - myTicks);
  else
    MatrixIntensity(myTicks);
}

Ticker myTick;

void setup() {
  MatrixInit();
  animWifi();
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    animWifi();
  }
  animwifiIndex = 2;
  animWifi();

  ArduinoOTA.setHostname(host);
  ArduinoOTA.begin();
  
  myTick.attach_ms(200, myeventtick);
}

int delayloop = 0;

void loop() {
  ArduinoOTA.handle();

  if (delayloop == 0){
    initialized = GetTime(&hour, &minute, &seconde);
    if (initialized == HTTPTime_OK) {
      MatrixShow();
      delayloop = 180000; //3 min
    }
    else
      delayloop = 30000; //30s
  }
  delayloop--;
  delay(1);
}
