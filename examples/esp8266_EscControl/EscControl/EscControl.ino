/********************
Arduino generic menu system

Feb. 2019 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

IMPORTANT!:
this requires the data folder to be stored on esp8266 spiff
Extra libraries should be present

arduinoWebSockets - https://github.com/Links2004/arduinoWebSockets
ESP8266WiFi - https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
ESP8266WebServer - https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer

*/
#include <Servo.h>

#include <menu.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/xmlFmt.h>//to write a menu has html page
#include <menuIO/serialIn.h>
#include <menuIO/xmlFmt.h>//to write a menu has xml page
#include <menuIO/jsonFmt.h>//to write a menu has xml page
#ifndef ARDUINO_STREAMING
  #include <streamFlow.h>//https://github.com/neu-rah/streamFlow
#else
  #include <Streaming.h>//https://github.com/scottdky/Streaming
#endif
//#include <menuIO/jsFmt.h>//to send javascript thru web socket (live update)
#include <FS.h>
#include <Hash.h>
extern "C" {
  #include "user_interface.h"
}

using namespace Menu;

#ifdef WEB_DEBUG
  // on debug mode I put aux files on external server to allow changes without SPIFF update
  // on this mode the browser MUST be instructed to accept cross domain files
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

//this version number MUST be the same as data/*
#define CUR_VERSION "1.5"
#define APName "EscControl"

#define ESCPIN 2
Servo myservo;

#ifndef MENU_SSID
  #error "need to define WiFi SSID here"
  #define MENU_SSID "r-site.net"
#endif
#ifndef MENU_PASS
  #error "need to define WiFi password here"
  #define MENU_PASS ""
#endif

// char wifiSSID[wifiSSIDLen+1];//="                                ";
// char wifiPwd [wifiPwdLen+1];//="                                ";

const char* ssid = MENU_SSID;
const char* password = MENU_PASS;
// const char* serverName="192.168.1.79";

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

int speed=0;
result setEsc() {
  myservo.write(map(speed,0,100,90,170));//this parameters are for my esc, please adapt
}
result armEsc() {
  // arm the speed controller, modify as necessary for your ESC
  speed=0;
  setEsc();
  delay(1000); //delay 1 second,  some speed controllers may need longer
  Serial.println("arming ESC");
  serverOut<<"arming ESC<br/>";
  return proceed;
}
result stopEsc() {
  speed=0;
  setEsc();
  Serial.println("ESC stop");
  serverOut<<"ESC stop<br/>";
  return proceed;
}

//the menu
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Stop",stopEsc,enterEvent)
  ,OP("Arm",armEsc,enterEvent)
  ,FIELD(speed,"Speed","%",0,100,10,1, setEsc, enterEvent, noStyle)
  ,EXIT("Exit!")
);

result idle(menuOut& o,idleEvent e) {
  //if (e==idling)
  Serial.println("suspended");
  o<<"suspended..."<<endl<<"press [select]"<<endl<<"to continue"<<endl<<(millis()%1000);
  return quit;
}

template<typename T>//some utill to help us calculate array sizes (known at compile time)
constexpr inline size_t len(T& o) {return sizeof(o)/sizeof(decltype(o[0]));}

//serial menu navigation
MENU_OUTLIST(out,&serverOut);
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//xml+http navigation control
noInput none;//web uses its own API
menuOut* web_outputs[]={&serverOut};
outputsList web_out(web_outputs,len(web_outputs));
navNode web_cursors[MAX_DEPTH];
navRoot webNav(mainMenu, web_cursors, MAX_DEPTH, none, web_out);

//json+http navigation control
menuOut* json_outputs[]={&jsonOut};
outputsList json_out(json_outputs,len(json_outputs));
navNode json_cursors[MAX_DEPTH];
navRoot jsonNav(mainMenu, json_cursors, MAX_DEPTH, none, json_out);

//websockets navigation control
menuOut* ws_outputs[]={&wsOut};
outputsList ws_out(ws_outputs,len(ws_outputs));
navNode ws_cursors[MAX_DEPTH];
navRoot wsNav(mainMenu, ws_cursors, MAX_DEPTH, none, ws_out);

//config myOptions('*','-',defaultNavCodes,false);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      //USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        //USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "console.log('ArduinoMenu Connected')");
      }
      break;
    case WStype_TEXT: {
        //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
        // nav.async((const char*)payload);//this is slow!!!!!!!!
        __trace(Serial.printf("[%u] get Text: %s\n", num, payload));
        char*s=(char*)payload;
        _trace(Serial<<"serve websocket menu"<<endl);
        wsOut.response.remove(0);
        wsOut<<"{\"output\":\"";
        wsNav.async((const char*)payload);
        wsOut<<"\",\n\"menu\":";
        wsNav.doOutput();
        wsOut<<"\n}";
        webSocket.sendTXT(num,wsOut.response);
        // wsOut.response.remove(0);
        // jsonEnd();
      } break;
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
        //Serial<<"id:"<<id<<endl;
        if (id==nav.active().hash()) {
          //Serial<<"id ok."<<endl;Serial.flush();
          //Serial<<"input:"<<inp<<endl;
          //StringStream inStr(inp);
          //while(inStr.available())
          nav.doInput(inp);
          webSocket.sendTXT(num, "binBusy=false;");//send javascript to unlock the state
        } //else Serial<<"id not ok!"<<endl;
        //Serial<<endl;
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

//redirect to version folder,
//this allows agressive caching with no need to cache reset on version change
auto mainPage= []() {
  _trace(Serial<<"serving main page from root!"<<endl);
  server.sendHeader("Location", CUR_VERSION "/index.html", true);
  server.send ( 302, "text/plain", "");
  if (server.hasArg("at"))
    nav.async(server.arg("at").c_str());
};

void setup(){
  Serial.begin(115200);
  while(!Serial);

  myservo.attach(ESCPIN);

  webNav.canExit=false;
  jsonNav.canExit=false;
  wsNav.canExit=false;

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  Serial.println();
  Serial.println("Arduino menu webserver example");

  SPIFFS.begin();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  // Wait for connection
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

  nav.idleTask=idle;//point a function to be used when menu is suspended

  server.on("/",HTTP_GET,mainPage);

  //menu xml server over http
  server.on("/menu", HTTP_GET, []() {
    pageStart();
    serverOut<<"<output state=\""<<((int)&webNav.idleTask)<<"\"><![CDATA[";
    _trace(Serial<<"output count"<<webNav.out.cnt<<endl);
    handleMenu(webNav);//do navigation (read input) and produce output messages or reports
    serverOut<<"]]></output>";
    webNav.doOutput();
    pageEnd();
  });

  //menu json server over http
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
  wsOut.response.remove(0);//clear websocket json buffer
  webSocket.loop();
  server.handleClient();
  delay(1);
}
