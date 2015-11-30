#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "Afficheur.h"
#include "HTTPTime.h"

#include "PrivateWifi.h"
//const char* ssid     = "....";
//const char* password = "....";
const char* host     = "esp8266-ota";
const uint16_t aport = 8266;

WiFiUDP OTA;

byte seconde;
byte myTicks;
byte initialized;
// Use WiFiClient class to create TCP connections

// Passer en seconde
void toto(){
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
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    animWifi();
  }
  animwifiIndex = 2;
  animWifi();

  MDNS.begin(host);
  MDNS.addService("arduino", "tcp", aport);
  OTA.begin(aport);
  
  myTick.attach_ms(200, toto);
}

int delayloop = 0;

void loop() {
  //OTA Sketch
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

//    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }

      if(Update.end()){
        client.println("OK");
        ESP.restart();
      } else {
        Update.printError(client);
      }
    }
  }

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
