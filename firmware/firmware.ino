//Libraries
#include <ESP8266WiFi.h>

//Variables
const char* ssid = "TestNetwork";
const char* passwd = "12345678";
const boolean debug = true;
const int serialSpeed = 9600;
const String ProgramVersion = "0.0.1 - 58fa97d";
WiFiServer tcpserver(80);

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



void setup(){
  sendDebug('i', "Module started");
  
  //Wake up methods
  connectWiFi();
  tcpserver.begin();

}

void loop(){
  if(checkConnection()){
    WiFiClient client = tcpserver.available();
    if(client){
      sendDebug('i', "[SERVER] New client connected.");
      sendDebug('i', "[SERVER] Client IP: "+client.remoteIP().toString());
      while (client.connected()){
      // Get client request
        if (client.available()){
          String line = client.readStringUntil('\r');
          switch(requestGetVariables(line)){
            case 0:
              sendDebug('i', "[SERVER] GPIO0 detected.");
            break;
            case 1:
              sendDebug('i', "[SERVER] GPIO1 detected.");
            break;
            case -1:
              sendDebug('i', "[SERVER] Input/output invalid");
            break;
            default:
              sendDebug('e', "[SERVER] requestGetVariables():1 internal exception.");
            break;
          }
          
          if(line.length() == 1 && line[0] == '\n'){
            client.println(htmlGenerator(200, 1, 1));
            break;
          }
        }
      }
    }
  }else{
    sendDebug('e', "Lost connection after started! Retry connection to WiFi network...");
    connectWiFi();
  }
}


void connectWiFi(){
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, passwd);
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

int requestGetVariables(String request){
  if(request.indexOf("/gpio/0") != -1){
    return 0;
  }else if(request.indexOf("/gpio/1") != -1){
    return 1;
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

//void sendBrowserResponse(String response){
//  client.println(completeResponse);
//}

boolean checkConnection(){
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  return true;
}
