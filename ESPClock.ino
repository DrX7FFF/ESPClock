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
//const char* host = "ESP_CLOCK";

//byte second;

byte initialized;
// Use WiFiClient class to create TCP connections

// Passer en seconde
void myeventtick(){
  myTicks++;
  if (myTicks == 10) {
    myTicks = 0;
    second++;
    if (second == 60) {
      second = 0;
      minute++;
      if (minute == 60) {
        minute = 0;
        hour++;
        if (hour == 24) {
          hour = 0;
        }
      }
    }
  }
  MatrixShow();
  /*
  if (second & 1)
    MatrixIntensity(5 - myTicks);
  else
    MatrixIntensity(myTicks);
    */
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

//  ArduinoOTA.setHostname(host);
  ArduinoOTA.begin();
  
  myTick.attach_ms(100, myeventtick);
}

int delayloop = 0;

void loop() {
  ArduinoOTA.handle();

  if (delayloop == 0){
    initialized = GetTime(&hour, &minute, &second);
    if (initialized == HTTPTime_OK) {
      MatrixShow();
      delayloop = 36000; //60 min
    }
    else
      delayloop = 300; //30s
  }
  delayloop--;
  delay(100);
}
