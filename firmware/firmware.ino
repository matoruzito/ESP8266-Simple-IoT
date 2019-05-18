//Libraries
#include <ESP8266WiFi.h>

//Variables
const char* ssid = "TestNetwork";
const char* passwd = "12345678";
const boolean debug = true;
const int serialSpeed = 9600;
const char* ProgramVersion = "0.0.1 - e2b8fb3";
WiFiServer serverip(80);

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

}

void loop(){
  Serial.println("LOOP");
  delay(4000);
}


void connectWiFi(){
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, passwd);
    sendDebug('i', "Connecting to WiFi network");
    while(WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED){
      Serial.println(WiFi.status());
      delay(500);
    }
    if(WiFi.status() == WL_CONNECTED){
      sendDebug('i', "Connected correctly.");
    }else if(WiFi.status() == WL_CONNECT_FAILED){
      sendDebug('e', "Error connecting to the network. (¿SSID out of range?)");
    }else{
      sendDebug('e', "Unexpected error connecting to network");
    }
  }else{
    sendDebug('w', "Connection request canceled. The WiFi module is already connected to a network.");
  }
}
