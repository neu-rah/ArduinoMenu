#include <menu.h>
#include <dev/serialOut.h>
#include <dev/htmlOut.h>
#include <ESP8266WiFi.h>

using namespace Menu;

#define SERVER_PORT 80
#define AP_SSID "r-site.net"
#define AP_PWD "rsite.2011"

WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"Menu 3.0 on ESP8266WiFi"<<endl;
  WiFi.begin(AP_SSID, AP_PWD);
  Serial<<"Connecting";
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial<<".";
  }
  Serial<<endl;

  Serial<<"Connected, IP address: "<<WiFi.localIP()<<endl;

  // Start the server
  server.begin();
  Serial<<"Server started at port "<<SERVER_PORT<<endl;;
  Serial.println(WiFi.localIP());
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,EXIT("<Back")
);

serialOut outSerial(Serial);//the output device (just the serial port)
menuOut* outputs[]={&outSerial};
outputsList out(outputs,1);
NAVROOT(nav,mainMenu,2,Serial,out);

void loop() {
  nav.poll();
}
