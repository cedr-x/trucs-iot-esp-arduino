#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

unsigned int STATE = 100;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

const int SW = 5;
//const int SW = 2;

WiFiServer server(80);
String header;

void setup() {
  pinMode(SW, OUTPUT);
  analogWrite(SW, STATE);
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.hostname("ampoule");
  WiFi.begin("wanadric","***/edited/***");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setPassword("ght1led");
  ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
  ArduinoOTA.begin();

  server.begin();  

}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {   
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html><html>");            
            client.println("<body><p><a href=\"/on=255\"><button>ON</button></a></p><p><a href=\"/on=100\"><button>Mi-on</button></a></p><p><a href=\"/on=0\"><button>OFF</button></a></p></body>");
            client.println("<script>document.body.addEventListener('wheel', myFunction);const m=40;var myImage = new Image();var vol=254*m;function myFunction(e) {this.style.backgroundColor='rgb('+vol/m+', '+vol/m+', '+vol/m+')';vol+=parseInt(e.deltaY);if (vol<0) vol=0;if (vol>254*m) vol=254*m;myImage.src = 'on='+vol/m;}</script> //merde a celui qui lit");
            client.println("</html>");
            if (header.indexOf("GET /on") >= 0) { 
              STATE = header.substring(header.indexOf("=")+1,header.indexOf(" ",header.indexOf("="))).toInt();
              analogWrite(SW, STATE);
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop(); 
  }
  
  ArduinoOTA.handle();
}
