#include <Arduino.h>
#include <espMenu.h>
#include <ESP8266WiFi.h>

//using namespace Menu;

#define SERVER_PORT 80
#define AP_SSID "r-site.net"
#define AP_PWD "rsite.2011"

WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 3.0 on ESP8266WiFi");
  WiFi.begin(AP_SSID, AP_PWD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.print("Server started at port ");
  Serial.println(SERVER_PORT);
  Serial.println(WiFi.localIP());
}

/*MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,EXIT("<Back")
);

serialOut outSerial(Serial);//the output device (just the serial port)
menuOut* outputs[]={&outSerial};
outputsList out(outputs,1);
NAVROOT(nav,mainMenu,2,Serial,out);*/

void loop() {
  //nav.poll();
  Serial.print(".");
  delay(500);
}
