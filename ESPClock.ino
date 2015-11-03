#include <Ticker.h>
#include <SPI.h>
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

//WiFiServer TelnetServer(aport);
//WiFiClient Telnet;
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
  Serial.begin(115200); // For debugging output
  Serial.println("");
  Serial.println("ESPClock V3");
  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
  
  MatrixInit();
  animWifi();
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
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
 // TelnetServer.begin();
 // TelnetServer.setNoDelay(true);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
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

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }

  if (delayloop == 0){
    initialized = GetTime(&hour, &minute, &seconde);
    Serial.print("Resultat synchro :");
    Serial.println(initialized);
    if (initialized == HTTPTime_OK) {
      MatrixShow();
      delayloop = 180000; //3 min
    }
    else
      delayloop = 30000; //30s
    Serial.print("Attente de (ms):");
    Serial.println(delayloop);
  }
  delayloop--;
  delay(1);
}
