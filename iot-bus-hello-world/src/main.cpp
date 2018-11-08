#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
  
const char* ssid = "........";
const char* password =  "........";
  
AsyncWebServer server(80);
  
void setup(){
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<p>Hello World!</p>");
  });

  server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<p>Some HTML!</p>");
  });
  
  server.begin();
}
  
void loop(){
}
