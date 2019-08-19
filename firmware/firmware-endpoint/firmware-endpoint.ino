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
  //EEPROM.get(addrEEPROM,Config);
  
  //¿First start?
  if(Config.previousConfig < 1){
    sendDebug('i', "No config detected... ¡Starting in configuration mode!");
    birthStart();
  }else{
    //Wake up methods
    sendDebug('i', "[FIRMWARE] Data loaded successful.");
    connectWiFi();
    startUDP();
    
    tcpserver.on("/exec", handleExecServer);
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
  handleUDPBroadcast();
}


/* 
 *  DEBUG FUNCTION
 */
void sendDebug(char type, String message){
  if(!Serial){
    Serial.begin(serialSpeed);
  }
  if(debug){
    switch(type){
      case 'e':
        Serial.println("[ERROR] "+message);
      break;
      case 'w':
        Serial.println("[WARN] "+message);  
      break;  
      case 'i':
        Serial.println("[INFO] "+message);
      break;
    }
  }
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
  tcpserver.on("/configuration", handleConfigurationServer);
  sendDebug('i', "Server started. Waiting configuration...");
}

/*
 * Connect WiFi method
 */
void connectWiFi(){
  if(WiFi.status() != WL_CONNECTED){
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

/*
 * 
 * HANDLER SERVER CONFIGURATION
 * 
 */
void handleConfigurationServer(){

}

void handleExecServer(){
  
}


int requestGetVariables(String request){
  if(request.indexOf("/gpio/0") != -1){
    return 0;
  }else if(request.indexOf("/gpio/1") != -1){
    return 1;
  }else if(request.indexOf("/config") != -1){
    return 9;
  }else{
    return -1;
  }
}

String htmlGenerator(int header, int htmlheader, int htmlbody){
  String header200 = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n Connection: close\r\n";
  String header500 = "HTTP/1.1 500";
  String basicHtmlHeader = "<html><head><title>ESP8266-Simple-IoT</title></head>";
  String basicHtmlBody = "<body><h2>Welcome to ESP8266-Simple-IoT</h2><h4> v"+ ProgramVersion +"</h4>";
  String htmlGenerated = "";
  switch(header){
    case 200:
      htmlGenerated += header200;
    break;
    case 500:
    default:
      htmlGenerated += header500;
    break;
  }

  switch(htmlheader){
    case 1:
    default:
      htmlGenerated += basicHtmlHeader;
    break;
  }

  switch(htmlbody){
    case 1:
    default:
      htmlGenerated += basicHtmlBody;
    break;
  }

  return htmlGenerated;
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

void handleUDPBroadcast(){
  int packetSize = udpBroadcast.parsePacket();
  if (packetSize) receivePacket(packetSize);
}

void startUDP(){
  if(udpBroadcast.begin(Config.listenerPort)){
    sendDebug('i', "UDP packet module started!");  
  }
}

bool sendDiscoverPacket(const IPAddress& address, const uint8_t* buf, uint8_t bufSize) {
  startUDP();
  udpBroadcast.beginPacket(address, Config.listenerPort);
  udpBroadcast.write(buf, bufSize);
  return (udpBroadcast.endPacket() == 1);
}

void receivePacket(int packetSize) {
  int randomIDPacket = random(30000);
  
  String prefix = "[BROADCAST " + (String) randomIDPacket;
  sendDebug('i', prefix+"] Recived packet of size "+(String) packetSize);
  sendDebug('i', prefix+"] From: "+udpBroadcast.remoteIP().toString());

  udpBroadcast.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  sendDebug('i', prefix + "] Info: " + packetBuffer);
  
  deserializeJson(doc, packetBuffer);
  sendDebug('e', doc["device"]); //Modo debug, recibir "gateway" en conjunto con un tipo "beacon" sería una baliza para busqueda automática.
  sendDebug('e', doc["type"]); 
  /*
   * Buscamos paquetes de estructura simil
   * {"device":"gateway", "type": "beacon"}
   */
  udpBroadcast.flush();

}
