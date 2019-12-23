#define UDPMODULE
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

  decodeJsonPacket(packetBuffer);
  
  /*
   * Buscamos paquetes de estructura simil
   * {"device":"gateway", "type": "beacon"}
   */
  udpBroadcast.flush();

}

void decodeJsonPacket(char packet[UDP_TX_PACKET_MAX_SIZE]){
  DeserializationError jsonDecodeError = deserializeJson(doc, packet);
  if (jsonDecodeError){
      String jsonDecodeErrorLog = "Error to trying decode json in decodeJsonPacket(); Error code: ";
      sendDebug('e', jsonDecodeErrorLog + jsonDecodeError.c_str());
      return;
  }
  sendDebug('i', "Decoding JSON packet...");

  if(doc['type'] != NULL){
    if(doc['type'] == "beacon" && doc['device'] == "gateway"){
      // Discover device packet
      //-----------------------------------------------Add to beacons array.  
    }
  }
}
