/*
This was a test file/example
its abandon in favor of **es8266/WebServer** example
because this does direct print and does not allow output capturing
it works ok for options
but it is not good for fields reflecivity
also a strange need of delay at end that i overlooked... its abandoned!
*/


#include <Arduino.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/htmlFmt.h>
//#include <Streaming.h>

#define max(a,b) (((a)>(b))?(a):(b))
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/esp8266Out.h>

using namespace Menu;

#define SERVER_PORT 80
#define AP_SSID "r-site.net"
#define AP_PWD "rsite.2011"

WiFiServer server(SERVER_PORT);

enum httpResCodes {ok,docHtml,e404,noCache,nrHttpResCodes};
const char * httpHeaders[nrHttpResCodes]={
  "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n",
  "<!DOCTYPE HTML>\r\n",
  "HTTP/1.0 404 Not Found\r\n",
  "Cache-Control: no-cache, no-store, must-revalidate\r\nPragma: no-cache\r\nExpires: 0\r\n",
};

#define BLACK "#000"
#define BLUE "navy"
#define GRAY "#888"
#define WHITE "white"
#define RED "red"
#define YELLOW "gold"

const colorDef<esp8266Out::webColor> colors[] MEMMODE={
  {{BLACK,BLACK},{BLACK,BLUE,BLUE}},//bgColor
  {{GRAY,GRAY},{WHITE,WHITE,WHITE}},//fgColor
  {{WHITE,BLACK},{YELLOW,YELLOW,RED}},//valColor
  {{WHITE,BLACK},{WHITE,YELLOW,YELLOW}},//unitColor
  {{WHITE,GRAY},{BLACK,BLUE,WHITE}},//cursorColor
  {{WHITE,YELLOW},{BLUE,RED,RED}},//titleColor
};

#define MAX_DEPTH 2
idx_t tops[MAX_DEPTH];
PANELS(webPanels,{0,0,20,50});
htmlFmt<esp8266_WiFiClientOut> serverOut(colors,tops,webPanels);

//menu action functions
result action1(eventMask event, navNode& nav, prompt &item) {
  Serial.println("action A called!");
  serverOut<<"This is action <b>A</b> web report "<<(millis()%1000)<<"<br/>";
  return proceed;
}
result action2(eventMask event, navNode& nav, prompt &item) {
  Serial.println("action B called!");
  serverOut<<"This is action <b>B</b> web report "<<(millis()%1000)<<"<br/>";
  return proceed;
}

int ledCtrl=LOW;

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

//the menu
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
);

MENU_OUTLIST(out,&serverOut);
NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

config myOptions={'*','-',false,false,defaultNavCodes};

void setup() {
  options=&myOptions;
  Serial<<".";
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 3.0 on ESP8266WiFi web server");
  Serial.print("Connecting to ");
  Serial.println(AP_SSID);
  Serial.flush();
  WiFi.begin(AP_SSID, AP_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.print("Server listens at ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(SERVER_PORT);
  //Serial<<"use "<<WiFi.localIP()<<"/menu to access"<<endl;
}

void loop() {
  WiFiClient client = server.available();
  if(!client) return;
  //Serial<<"reading client"<<endl;
  // Read the first line of the request
  String req = client.readStringUntil('\r');

  // Match the request
  if (req.startsWith("GET ")) {
    if (req.startsWith("GET /favicon.ico")) {
      client.print(httpHeaders[e404]);
    } else {
      //Serial.println(req);
      serverOut.client=&client;
      client.print(httpHeaders[ok]);
      client.print(httpHeaders[noCache]);
      client<<endl;
      client.print(httpHeaders[docHtml]);
      client<<"<html><head><title>ArduinoMenu library OTA</title></head><body>";
      if(req.length()>5) {
        uint8_t ch=req[5];
        //Serial<<"ch:"<<ch<<endl;
        if (ch>='0'&&ch<='9') {
          nav.doNav(navCmd(selCmd,ch));
          nav.doOutput();
          client<<"<hr/>";
          nav.doNav(navCmd(enterCmd));//this is not good, menu already printed and status does not reflewct the action!
          delay(1);
        } else nav.doOutput();
      } else nav.doOutput();
      client<<"</body></html>";
      serverOut.client=NULL;
    }// else client.print(httpHeaders[e404]);
  } else client.print(httpHeaders[e404]);
  delay(500);//this is not good, arbitrary delay not always enuf for the page... need a termination check
}
