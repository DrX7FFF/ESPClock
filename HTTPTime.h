#define timePort  80
#define timeHost "www.worldtimeserver.com"
#define HTTPTime_OK           0x1
#define HTTPTime_ErrorConnect 0x2
#define HTTPTime_ErrorNoData  0x3
#define HTTPTime_ErrorParse   0x4

byte GetTime(byte* hour, byte* minute, byte* second){
  WiFiClient client;
  if (!client.connect(timeHost, timePort)){
    client.stop();
    return HTTPTime_ErrorConnect;
  }

  client.print("GET /handlers/GetData.ashx?action=GCTData HTTP/1.1\r\nConnection: keep-alive\r\nUser-Agent: ESP8266Clock\r\nHost: ");
  client.println(timeHost);
  client.println();

  for (int i=0;i<100;i++){
    if (client.available())
      break;
    delay(100);
  }
  while(client.available() && !client.readStringUntil('\n').equals("\r")){
  }
  
  char json[800];
  int size = client.readBytes(json,799);
  client.stop();
  if (size==0)
    return HTTPTime_ErrorNoData;
  
  json[size] = '\0';
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success())
    return HTTPTime_ErrorParse;
  
  const char* DH = root["DateTime_24HR"];
  *hour = atoi(DH);
  *minute = atoi(DH+3);
  *second = atoi(DH+6);
  return HTTPTime_OK;
}

