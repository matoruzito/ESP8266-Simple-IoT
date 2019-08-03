//Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//Variables & Objetos
const boolean debug = true;
const int serialSpeed = 9600;
const String ProgramVersion = "0.0.1 - c178958";
uint addrEEPROM = 0;

struct {
  int previousConfig = 0;
  int modeGPIO0 = -1;
  int modeGPIO1 = -1;
  char* ssid = "default";
  char* passwd = "none";
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
  if(Config.previousConfig != 1){
    sendDebug('i', "No config detected... ¡Starting in configuration mode!");
    birthStart();
  }else{
    //Wake up methods
    connectWiFi();
    sendDebug('i', "[FIRMWARE] Data loaded successful.");
    tcpserver.on("/exec", handleExecServer);
  }

  /*EEPROM.put(addrEEPROM,Config);
  EEPROM.commit();
  EEPROM.end();*/

  tcpserver.begin();
  sendDebug('i', "POST Pass");
}

void loop(){
  if(checkConnection()){

  }else{
    sendDebug('e', "Lost connection after started! Retry connection to WiFi network...");
    connectWiFi();
  }
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
  while(!WiFi.softAP("SimpleIoT-ENDPOINT", "12345678")){}
  sendDebug('i', "AP started! IP: "+WiFi.softAPIP());
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
