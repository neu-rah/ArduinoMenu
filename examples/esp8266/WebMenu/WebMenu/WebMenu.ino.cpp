# 1 "/tmp/tmpTgRVqU"
#include <Arduino.h>
# 1 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/esp8266/WebMenu/WebMenu/WebMenu.ino"
# 28 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/esp8266/WebMenu/WebMenu/WebMenu.ino"
#include <menu.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/xmlFmt.h>
#include <menuIO/serialIn.h>
#include <menuIO/xmlFmt.h>
#include <menuIO/jsonFmt.h>

#include <streamFlow.h>

#include <FS.h>
#include <Hash.h>
extern "C" {
  #include "user_interface.h"
}

using namespace Menu;

#ifdef WEB_DEBUG


  String xslt("http://neurux:8080/");
#else
  String xslt("");
#endif

menuOut& operator<<(menuOut& o,unsigned long int i) {
  o.print(i);
  return o;
}
menuOut& operator<<(menuOut& o,endlObj) {
  o.println();
  return o;
}

#define CUR_VERSION "1.0"
#define APName "WebMenu"
#define ANALOG_PIN 4

constexpr size_t wifiSSIDLen=64;
constexpr size_t wifiPwdLen=32;

#ifndef MENU_SSID
  #error "need to define WiFi SSID here"
  #define MENU_SSID "r-site.net"
#endif
#ifndef MENU_PASS
  #error "need to define WiFi password here"
  #define MENU_PASS ""
#endif

char wifiSSID[wifiSSIDLen+1];
char wifiPwd [wifiPwdLen+1];

const char* ssid = MENU_SSID;
const char* password = MENU_PASS;
const char* serverName="192.168.1.79";

#define HTTP_PORT 80
#define WS_PORT 81
#define USE_SERIAL Serial
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

#define MAX_DEPTH 2
idx_t web_tops[MAX_DEPTH];
PANELS(webPanels,{0,0,80,100});
xmlFmt<esp8266_WebServerStreamOut> serverOut(server,web_tops,webPanels);
jsonFmt<esp8266_WebServerStreamOut> jsonOut(server,web_tops,webPanels);
jsonFmt<esp8266BufferedOut> wsOut(web_tops,webPanels);
result action1(eventMask event, navNode& nav, prompt &item);
result action2(eventMask event, navNode& nav, prompt &item);
void updLed();
void updAnalog();
result idle(menuOut& o,idleEvent e);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void pageStart();
void pageEnd();
void jsonStart();
void jsonEnd();
bool handleMenu(navRoot& nav);
void setup();
void loop(void);
#line 99 "/home/azevedo/Sketchbook/LIBDEV/ArduinoMenu/examples/esp8266/WebMenu/WebMenu/WebMenu.ino"
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
#define LEDPIN LED_BUILTIN
void updLed() {

}

TOGGLE(ledCtrl,setLed,"Led: ",updLed,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

int timeOn=50;
void updAnalog() {

}


MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
  ,FIELD(timeOn,"On","ms",0,100,10,1, updAnalog, anyEvent, noStyle)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
);

result idle(menuOut& o,idleEvent e) {

  Serial.println("suspended");
  o<<"suspended..."<<endl<<"press [select]"<<endl<<"to continue"<<endl<<(millis()%1000);
  return quit;
}

template<typename T>
constexpr inline size_t len(T& o) {return sizeof(o)/sizeof(decltype(o[0]));}


MENU_OUTLIST(out,&serverOut);
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);


noInput none;
menuOut* web_outputs[]={&serverOut};
outputsList web_out(web_outputs,len(web_outputs));
navNode web_cursors[MAX_DEPTH];
navRoot webNav(mainMenu, web_cursors, MAX_DEPTH, none, web_out);


menuOut* json_outputs[]={&jsonOut};
outputsList json_out(json_outputs,len(json_outputs));
navNode json_cursors[MAX_DEPTH];
navRoot jsonNav(mainMenu, json_cursors, MAX_DEPTH, none, json_out);


menuOut* ws_outputs[]={&wsOut};
outputsList ws_out(ws_outputs,len(ws_outputs));
navNode ws_cursors[MAX_DEPTH];
navRoot wsNav(mainMenu, ws_cursors, MAX_DEPTH, none, ws_out);



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:

      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);

        webSocket.sendTXT(num, "console.log('ArduinoMenu Connected')");
      }
      break;
    case WStype_TEXT:

      nav.async((const char*)payload);
      break;
    case WStype_BIN: {
        USE_SERIAL<<"[WSc] get binary length:"<<length<<"[";
        for(int c=0;c<length;c++) {
          USE_SERIAL.print(*(char*)(payload+c),HEX);
          USE_SERIAL.write(',');
        }
        USE_SERIAL<<"]"<<endl;
        uint16_t id=*(uint16_t*) payload++;
        idx_t len=*((idx_t*)++payload);
        idx_t* pathBin=(idx_t*)++payload;
        const char* inp=(const char*)(payload+len);

        if (id==nav.active().hash()) {




          nav.doInput(inp);
          webSocket.sendTXT(num, "binBusy=false;");
        }

      }
      break;
    default:break;
  }
}

void pageStart() {
  _trace(Serial<<"pasgeStart!"<<endl);
  serverOut<<"HTTP/1.1 200 OK\r\n"
    <<"Content-Type: text/xml\r\n"
    <<"Connection: close\r\n"
    <<"Expires: 0\r\n"
    <<"\r\n";
  serverOut<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\r\n"
    "<?xml-stylesheet type=\"text/xsl\" href=\"";
  serverOut<<xslt;
  serverOut<<CUR_VERSION"/device.xslt";
  serverOut<<"\"?>\r\n<menuLib"
    #ifdef WEB_DEBUG
      <<" debug=\"yes\""
    #endif
    <<" host=\"";
    serverOut.print(APName);
    serverOut<<"\">\r\n<sourceURL ver=\"" CUR_VERSION "/\">";
  if (server.hasHeader("host"))
    serverOut.print(server.header("host"));
  else
    serverOut.print(APName);
  serverOut<<"</sourceURL>";
}

void pageEnd() {
  serverOut<<"</menuLib>";
  server.client().stop();
}

void jsonStart() {
  _trace(Serial<<"jsonStart!"<<endl);
  serverOut<<"HTTP/1.1 200 OK\r\n"
    <<"Content-Type: application/json; charset=utf-8\r\n"
    <<"Connection: close\r\n"
    <<"Expires: 0\r\n"
    <<"\r\n";
}

void jsonEnd() {
  server.client().stop();
}

bool handleMenu(navRoot& nav){
  _trace(
    uint32_t free = system_get_free_heap_size();
    Serial.print(F("free memory:"));
    Serial.print(free);
    Serial.print(F(" handleMenu "));
    Serial.println(server.arg("at").c_str());
  );
  String at=server.arg("at");
  bool r;
  r=nav.async(server.hasArg("at")?at.c_str():"/");
  return r;
}



auto mainPage= []() {
  _trace(Serial<<"serving main page from root!"<<endl);
  server.sendHeader("Location", CUR_VERSION "/index.html", true);
  server.send ( 302, "text/plain", "");
};

void setup(){







  Serial.begin(115200);
  while(!Serial)

  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }






  Serial.println("");
  Serial.println("Arduino menu webserver example");

  SPIFFS.begin();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  webSocket.begin();
  Serial.println("");
  webSocket.onEvent(webSocketEvent);
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();

  nav.idleTask=idle;

  server.on("/",HTTP_GET,mainPage);


  server.on("/menu", HTTP_GET, []() {
    pageStart();
    serverOut<<"<output state=\""<<((int)&webNav.idleTask)<<"\"><![CDATA[";
    _trace(Serial<<"output count"<<webNav.out.cnt<<endl);
    handleMenu(webNav);
    serverOut<<"]]></output>";
    webNav.doOutput();
    pageEnd();
  });


  server.on("/json", HTTP_GET, []() {
    _trace(Serial<<"json request!"<<endl);
    jsonStart();
    serverOut<<"{\"output\":\"";
    handleMenu(jsonNav);
    serverOut<<"\",\n\"menu\":";
    jsonNav.doOutput();
    serverOut<<"\n}";
    jsonEnd();
  });

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Serving ArduinoMenu example.");
  #ifdef MENU_DEBUG
    server.serveStatic("/", SPIFFS, "/","max-age=30");
  #else
    server.serveStatic("/", SPIFFS, "/","max-age=31536000");
  #endif
}

void loop(void){
  wsOut.response.remove(0);
  webSocket.loop();
  server.handleClient();
  delay(1);
}