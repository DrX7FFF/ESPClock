// Version Bleue OTA : generic module / 80 Mhz / QIO / 40 MHz / 512k (0kb) / 230400
// Version Noire OTA : generic module / 80 Mhz / QIO / 40 MHz / 1M (0kb) / 230400
byte myTicks;

// Librarie de carte ESP 2.4.2
// Attention!!!
// Ajouter la bibliothéque : NTPClientLib by German Martin : Version Branche développement 3.0.0beta - 4 janvier 2019 (après la 2.5.1) 
// Besoin de la librairie : me-no-dev/ESPAsyncUDP : Version 1.0.0 - 14 juillet 2016

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "Afficheur.h"
#include <TimeLib.h>
#include <NtpClientLib.h>

#include "PrivateWifi.h"
// #define WIFI_SSID "...."
// #define WIFI_PASSWD "...."
#define HOSTNAME "CLOCK"

#define LISTEN_PORT 9978
WiFiServer server(LISTEN_PORT);
WiFiClient clientCur;

/*
//#define BOSEMON_NAME "BOSEMON"
#define BOSEMON_NAME "PORTABLE"
#define BOSEMON_PORT 9978
WiFiClient boseClient;
const char* boseName = "Portable";

//IPAddress boseIP(192, 168, 1, 23); //Portable
IPAddress boseIP(192, 168, 1, 99);
*/
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
  WiFi.hostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    animWifi();
  }
  animwifiIndex = 2;
  animWifi();

  NTP.begin("pool.ntp.org", 1, true);
  NTP.setInterval(30,3600);

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

  server.begin();
  
  myTick.attach_ms(100, myeventtick);
}

//#define WATCHDOG 30000
//unsigned long previousMillis;
uint8_t recvBuf[3];

void loop() {
//  uint8_t bosePowerChange;
//  uint8_t boseVolumeChange;
  ArduinoOTA.handle();

  //Nouvelle connexion remplace l'ancienne
  WiFiClient clientNew = server.available();
  if(clientNew){
    if (clientCur && clientCur.connected())
      clientCur.stop();
    clientCur = clientNew;
    clientCur.print("Started for (ms) :");
    clientCur.println(millis());
    clientCur.println(ESP.getResetReason());
  }
  
/*
  if (!boseClient.connected() || ( millis() - previousMillis > WATCHDOG )) {
    boseClient.stop();
//    connected2Bose = boseClient.connect(boseName, BOSEMON_PORT);
    connected2Bose = boseClient.connect(boseIP, BOSEMON_PORT);
    previousMillis = millis();
  }
  else{
    if (boseClient.available() >= sizeof(recvBuf)) {
      boseClient.read(recvBuf,sizeof(recvBuf));
      if (recvBuf[0] != 0x80)
          boseClient.stop();
      else {
        bosePowerChange = 0;
        boseVolumeChange = 0;
        if (boseOn != (recvBuf[1] & B00000001)){
          boseOn = (recvBuf[1] & B00000001);
          bosePowerChange = boseOn;
        }
        if (boseError != (recvBuf[1] & B00000010))
          boseError=(recvBuf[1] & B00000010);

        if (boseVolume != recvBuf[2]){
          boseVolume = recvBuf[2];
          boseVolumeChange = 1;
        }
      }
      if (bosePowerChange)
        MatrixShow(2);
      else
        if (boseVolumeChange)
          MatrixShow(1);
      previousMillis = millis();
    }
  }
*/
  delay(10);
}
