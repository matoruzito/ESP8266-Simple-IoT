//Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

//Variables & Objetos
const boolean debug = true;
const int serialSpeed = 9600;
const String ProgramVersion = "0.0.1 - c178958";
uint addrEEPROM = 0;
WiFiUDP udpBroadcast;
StaticJsonDocument<200> doc;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
IPAddress broadcastAddress;


struct {
  int previousConfig = 1;
  int modeGPIO0 = -1;
  int modeGPIO1 = -1;
  char* ssid = "TestNetwork";
  char* passwd = "12345678";
  int listenerPort = 19950;
} Config;

ESP8266WebServer tcpserver(Config.listenerPort);

void setup(){
  //Boot message
  String bootMessage = "Booting...\nESP8266-Simple-IoT v" + ProgramVersion;  
  sendDebug('i', bootMessage);

  //Load config
  EEPROM.begin(512);
  EEPROM.get(addrEEPROM,Config);
  
  //¿First start?
  if(Config.previousConfig < 1){
    sendDebug('i', "No config detected... ¡Starting in configuration mode!");
    birthStart();
  }else{
    //Wake up methods
    sendDebug('i', "[FIRMWARE] Data loaded successful.");
    connectWiFi();
    startUDP();
    
  }

  /*EEPROM.put(addrEEPROM,Config);
  EEPROM.commit();
  EEPROM.end();*/

  tcpserver.begin();
  sendDebug('i', "POST Pass");
}

void loop(){
  if(Config.previousConfig != 1){
    //Configuration mode handlers
  }else if(!checkConnection()){
    sendDebug('e', "Lost connection after started! Retry connection to WiFi network...");
    connectWiFi();
  }
  
  //Loop handlers
  handleSerial();
  #ifdef UDPMODULE
  handleUDPBroadcast(); 
  #endif
}

/*
 * First start
 */
void birthStart(){
  WiFi.mode(WIFI_AP_STA);
  sendDebug('i', "Waking up AP...");
  while(!WiFi.softAP("Simple-IoT-ENDPOINT", "12345678")){}
  String debugApIp = "AP Started! Gateway IP: " + WiFi.softAPIP().toString();
  sendDebug('i', debugApIp);

  sendDebug('i', "Server started. Waiting configuration...");
}

/*
 * Connect WiFi method
 */
void connectWiFi(){
  if(WiFi.status() != WL_CONNECTED){
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setOutputPower(20);
    WiFi.begin(Config.ssid, Config.passwd);
    sendDebug('i', "Connecting to WiFi network");
    while(WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED){
      sendDebug('i', ".");
      delay(500);
    }
    if(WiFi.status() == WL_CONNECTED){
      sendDebug('i', "Connected correctly.");
      sendDebug('i', "IP: "+WiFi.localIP().toString());
      broadcastAddress = (uint32_t)WiFi.localIP() | ~((uint32_t)WiFi.subnetMask());
      sendDebug('i', "Broadcast Address: "+broadcastAddress.toString());
    }else if(WiFi.status() == WL_CONNECT_FAILED){
      sendDebug('e', "Error connecting to the network. (¿SSID out of range?)");
    }else{
      sendDebug('e', "Unexpected error connecting to network");
    }
  }else{
    sendDebug('w', "Connection request canceled. The WiFi module is already connected to a network.");
  }
}

String readEEPROM(){
  
}

boolean checkConnection(){
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  return true;
}

void handleSerial() {
 while(Serial.available() > 0){
   char incomingChar = Serial.read();
   switch(incomingChar){
     case 'R':
      sendDebug('i', "[FIRMWARE] Action 'Restart' handled! Restarting device in 5 seconds");
      for(int i=4; i>0; i--){
        delay(1000);
        String tmpRestartTime = (String) i;
        String tmpRestartDebug = "Restarting device in " + tmpRestartTime + " seconds.";
        sendDebug('i', tmpRestartDebug);
      }
      ESP.restart();
     break;
    }
 }
}
