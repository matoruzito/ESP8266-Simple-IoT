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
