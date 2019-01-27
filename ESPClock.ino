// Version Bleue OTA : generic module / 80 Mhz / QIO / 40 MHz / 512k (0kb) / 230400
// Version Noire OTA : generic module / 80 Mhz / QIO / 40 MHz / 1M (0kb) / 230400
byte myTicks;

// Attention!!!
// Ajouter la bibliothéque : NTPClientLib by German Martin

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

#define DEBUG_PORT 9977
WiFiServer debugServer(DEBUG_PORT);
WiFiClient debugClient;

//#define BOSEMON_NAME "BOSEMON"
#define BOSEMON_NAME "Portable"
#define BOSEMON_PORT 9978
WiFiClient boseClient;

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 23);

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

  debugServer.begin();

  NTP.begin("pool.ntp.org", 1, true);
  NTP.setInterval(30,3600);

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();
  
  myTick.attach_ms(100, myeventtick);
}

void loop() {
  ArduinoOTA.handle();

  if(!debugClient.connected()) { // if client not connected
    debugClient = debugServer.available(); // wait for it to connect
    if (debugClient.connected()){
      debugClient.println("Debug connected");
      debugClient.println(sizeof(animBose));
    }
  }

//  if (boseClient.
// if the server's disconnected, stop the client:
  if (!boseClient.connected()) {
    error = 1;
    debugClient.println("Client disconnected");
    boseClient.stop();
    boseClient.connect(server, BOSEMON_PORT);
  }
  else
    error = 0;

  if (boseClient.available()) {
    char c = boseClient.read();
    debugClient.print(c);
    volume=74;
    if (c=='q')
      MatrixShow(2);
    else
      MatrixShow(1);
  }

  delay(10);  
}
