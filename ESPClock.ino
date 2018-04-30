// OTA : generic module / 80 Mhz / QIO / 40 MHz / 512k (0kb) / 230400
byte myTicks;

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "Afficheur.h"
#include <TimeLib.h>
#include <NtpClientLib.h>

#include "PrivateWifi.h"
// #define WIFI_SSID "...."
// #define WIFI_PASSWD "...."

void myeventtick(){
  myTicks = (myTicks+1) % 10;
  MatrixShow();
}

Ticker myTick;

void setup() {
  MatrixInit();
  animWifi();
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    animWifi();
  }
  animwifiIndex = 2;
  animWifi();

  NTP.begin("pool.ntp.org", 1, true);
  NTP.setInterval(30,3600);

  ArduinoOTA.begin();
  
  myTick.attach_ms(100, myeventtick);
}

void loop() {
  ArduinoOTA.handle();
  delay(10);
}
