/********************
Arduino generic menu system
XmlServer menu example
based on WebServer:
  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
  https://github.com/Links2004/arduinoWebSockets

Dec. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu on web browser served by esp8266 device (experimental)
output: ESP8266WebServer -> Web browser
input: ESP8266WebSocket <- Web browser
format: xml

this requires the data folder to be stored on esp8266 spiff
for development purposes some files are left external,
therefor requiring an external webserver to provide them (just for dev purposes)
i'm using nodejs http-server (https://www.npmjs.com/package/http-server)
to static serve content from the data folder. This allows me to quick change
the files without having to upload them to SPIFFS
also gateway ssid and password are stored on this code (bellow),
so don't forget to change it.
'dynamic' list of detected wi-fi and password request would require the new
added textField (also experimental).

*/

#include <menu.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/xmlFmt.h>//to write a menu has html page
#include <menuIO/serialIn.h>
#include <menuIO/xmlFmt.h>//to write a menu has xml page
#include <menuIO/jsonFmt.h>//to write a menu has xml page
// #include <Streaming.h>
#include <streamFlow.h>
//#include <menuIO/jsFmt.h>//to send javascript thru web socket (live update)
#include <FS.h>
#include <Hash.h>
extern "C" {
  #include "user_interface.h"
}

using namespace Menu;

#define CUR_VERSION 1.0
bool fromWeb=false;
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

char wifiSSID[wifiSSIDLen+1];//="                                ";
char wifiPwd [wifiPwdLen+1];//="                                ";

// result wifiScan(eventMask event, navNode& nav, prompt &item) {
//   int n=WiFi.scanNetworks(false);
//   Serial.println();
//   nav.target->dirty=true;
//   if (n == 0) {
//     Serial.println("no network found.";Serial.flush();
//     nav.root->idleOn((idleFunc)noWifiFound);
//   } else {
//     prompt** nets=new prompt*[n+1];
//     for (int i = 0; i < n; ++i) {
//       // Print SSID and RSSI for each network found
//       char *ssid = new char[WiFi.SSID(i).length() + 1];
//       strcpy(ssid, WiFi.SSID(i).c_str());
//       // Serial.print(i + 1);
//       // Serial.print(F(": "));
//       // Serial.print(ssid);
//       // Serial.print(F(" ("));
//       // Serial.print(WiFi.RSSI(i));
//       // Serial.print(F(")"));
//       // Serial.println((WiFi.encryptionType(i) == WIFI_OPEN)?" ":"*");
//       nets[i]=new prompt(ssid,selectWifi,enterEvent);
//     }
//     nets[n]=(prompt*)&back;
//     for (int i = 0; i < selNetworkMenu.sz()-1; i++)
//       delItem(((menuNodeShadow*)selNetworkMenu.shadow)->data[i]);
//     delete [] ((menuNodeShadow*)selNetworkMenu.shadow)->data;
//     ((menuNodeShadow*)selNetworkMenu.shadow)->data=nets;
//     ((menuNodeShadow*)selNetworkMenu.shadow)->sz=n+1;
//   }
//   // Serial.println(F(""));
//   return proceed;
// }

// SelNetMenu selNetworkMenu(
//   lang[txtSSID],
//   len(networkMenu_data),
//   networkMenu_data,wifiScan,enterEvent,wrapStyle
// );

const char* ssid = MENU_SSID;
const char* password = MENU_PASS;
const char* serverName="192.168.1.79";
#ifdef DEBUG
  // on debug mode I put aux files on external server to allow changes without SPIFF update
  // on this mode the browser MUST be instructed to accept cross domain files
  #define xslt "http://neurux:8080/menu.xslt"
#else
  #define xslt "/menu.xslt"
  #define endl "\r\n"
  menuOut& operator<<(menuOut&o, long unsigned int i) {
    o.print(i);
    return o;
  }
#endif

#define HTTP_PORT 80
#define WS_PORT 81
#define USE_SERIAL Serial
// ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

#define MAX_DEPTH 2
// idx_t tops[MAX_DEPTH];
// PANELS(webPanels,{0,0,30,20});
idx_t web_tops[MAX_DEPTH];
PANELS(webPanels,{0,0,80,100});
xmlFmt<esp8266_WebServerStreamOut> serverOut(server,web_tops,webPanels);
jsonFmt<esp8266_WebServerStreamOut> jsonOut(server,web_tops,webPanels);
jsonFmt<esp8266BufferedOut> wsOut(web_tops,webPanels);

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
#define LEDPIN LED_BUILTIN
void updLed() {
  digitalWrite(LEDPIN,!ledCtrl);
}

TOGGLE(ledCtrl,setLed,"Led: ",updLed,enterEvent,noStyle//,doExit,enterEvent,noStyle
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
  analogWrite(ANALOG_PIN,map(timeOn,0,100,0,255/*PWMRANGE*/));
}

//the menu
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
  ,FIELD(timeOn,"On","ms",0,100,10,1, updAnalog, anyEvent, noStyle)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
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
    case WStype_TEXT:
      //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      nav.async((const char*)payload);//this is slow!!!!!!!!
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
  }
}

void pageStart() {
  _trace(Serial<<"pasgeStart!"<<endl);
  serverOut<<"HTTP/1.1 200 OK\r\n"
    <<"Content-Type: text/xml\r\n"
    <<"Connection: close\r\n"
    <<"Expires: 0\r\n"
    // <<"Access-Control-Allow-Origin: *\r\n"
    <<"\r\n";
  serverOut<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\r\n"
    "<?xml-stylesheet type=\"text/xsl\" href=\"";
  serverOut<<"CUR_VERSION/device.xslt";
  serverOut<<"\"?>\r\n<menuLib"
    #ifdef WEB_DEBUG
      <<" debug=\"yes\""
    #endif
    <<" host=\"";
    serverOut.print(APName/*WiFi.localIP()*/);
    // serverOut<<"\">\r\n<sourceURL ver=\"\">";
    serverOut<<"\">\r\n<sourceURL ver=\"CUR_VERSION/\">";
  if (server.hasHeader("host"))
    serverOut.print(server.header("host"));
  else
    serverOut.print(APName/*WiFi.localIP()*/);
  serverOut<<"</sourceURL>";
}

void pageEnd() {
  serverOut<<"</menuLib>";
  server.client().stop();
}

// void pageStart() {
//   server.sendHeader("Cache-Control","no-cache, no-store, must-revalidate");
//   server.sendHeader("Pragma","no-cache");
//   server.sendHeader("Expires","0");
//   serverOut
//     <<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\r\n"
//       "<?xml-stylesheet type=\"text/xsl\" href=\""
//       xslt
//       "\"?>\r\n<menuLib>\r\n"
//       "<sourceURL>"
//     <<serverName
//     <<"</sourceURL>";
// }
//
// void pageEnd() {
//   serverOut<<"</menuLib>";
//   server.send(200, "text/xml", serverOut.response);
// }

void handleNotFound(){
  //digitalWrite(led, 1);
  Serial.println("404 not found");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  USE_SERIAL.println(message);
  //digitalWrite(led, 0);
}

// String getContentType(String filename){
//   if(server.hasArg("download")) return "application/octet-stream";
//   else if(filename.endsWith(".htm")) return "text/html";
//   else if(filename.endsWith(".html")) return "text/html";
//   else if(filename.endsWith(".css")) return "text/css";
//   else if(filename.endsWith(".js")) return "application/javascript";
//   else if(filename.endsWith(".png")) return "image/png";
//   else if(filename.endsWith(".gif")) return "image/gif";
//   else if(filename.endsWith(".jpg")) return "image/jpeg";
//   else if(filename.endsWith(".ico")) return "image/x-icon";
//   else if(filename.endsWith(".xml")) return "text/xml";
//   else if(filename.endsWith(".xsl")) return "text/xml";
//   else if(filename.endsWith(".xslt")) return "text/xsl";
//   else if(filename.endsWith(".pdf")) return "application/x-pdf";
//   else if(filename.endsWith(".zip")) return "application/x-zip";
//   else if(filename.endsWith(".gz")) return "application/x-gzip";
//   return "text/plain";
// }

// bool handleFileRead(String path){
//   if(path.endsWith("/")) path += "index.html";
//   String contentType = getContentType(path);
//   String pathWithGz = path + ".gz";
//   if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
//     if(SPIFFS.exists(pathWithGz))
//       path += ".gz";
//     File file = SPIFFS.open(path, "r");
//     size_t sent = server.streamFile(file, contentType);
//     file.close();
//     return true;
//   }
//   // Serial<<"file not found "<<path<<endl;
//   return false;
// }

// inline void notfound() {
//   server.send(404, "text/plain", "FileNotFound"+server.uri());
// }

// bool handleMenu(){
//   if (server.hasArg("at"))
//     return nav.async(server.arg("at").c_str());
//   else return true;
// }

void jsonStart() {
  _trace(Serial<<"jsonStart!"<<endl);
  // toJsonOut=true;
  serverOut<<"HTTP/1.1 200 OK\r\n"
    <<"Content-Type: application/json; charset=utf-8\r\n"
    <<"Connection: close\r\n"
    <<"Expires: 0\r\n"
    // <<"Access-Control-Allow-Origin: *\r\n"
    <<"\r\n";
  // server.sendHeader("Cache-Control","no-cache, no-store, must-revalidate");
  // server.sendHeader("Pragma","no-cache");
  // server.sendHeader("Expires","0");
  // serverOut<<"/*ArduinoMenu library json output*/\n";
}

void jsonEnd() {
  // toJsonOut=false;
  // serverOut<<"//end!";
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
  fromWeb=true;
  String at=server.arg("at");
  bool r;
  r=nav.async(server.hasArg("at")?at.c_str():"/");
  fromWeb=false;
  return r;
}

void setup(){
  pinMode(LEDPIN,OUTPUT);
  updLed();
  analogWriteRange(1023);
  pinMode(ANALOG_PIN,OUTPUT);
  updAnalog();
  //options=&myOptions;//menu options
  Serial.begin(115200);
  while(!Serial)
  //USE_SERIAL.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  // Serial.setDebugOutput(1);
  // Serial.setDebugOutput(0);
  // while(!Serial);
  // delay(10);
  // wifi_station_set_hostname((char*)serverName);
  Serial.println("");
  Serial.println("Arduino menu webserver example");

  SPIFFS.begin();

  //WiFi.hostname(serverName);//not good

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // WiFiMulti.addAP(ssid, password);
  // while(WiFiMulti.run() != WL_CONNECTED) delay(100);
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

  // if (MDNS.begin("esp8266")) {
  //   Serial.println("MDNS responder started");
  // }

  nav.idleTask=idle;//point a function to be used when menu is suspended

  //we have none, so do not ask again! use a standard...
  server.on("/favicon.ico",handleNotFound);

  // server.on("/", HTTP_GET, []() {
  //   handleMenu();//no output will be done on this mode
  //   if(!handleFileRead("/index.html")) notfound();}//static page
  // );

  //menu xml server over http
  server.on("/menu", HTTP_GET, []() {
    pageStart();
    serverOut<<"<output state=\""<<((int)&webNav.idleTask)<<"\"><![CDATA[";
    _trace(Serial<<"output count"<<webNav.out.cnt<<endl);
    handleMenu(webNav);//do navigation (read input) and produce output messages or reports
    serverOut<<"]]></output>";
    fromWeb=true;
    webNav.doOutput();
    fromWeb=false;
    pageEnd();
    // handleMenu();
    // String r(serverOut.response);
    // serverOut.response.remove(0);
    // pageStart();
    // nav.doOutput();
    // serverOut<<"<output>";
    // serverOut<<r;
    // serverOut<<"</output>";
    // pageEnd();
    // delay(1);
  });

  //menu json server over http
  server.on("/json", HTTP_GET, []() {
    _trace(Serial<<"json request!"<<endl);
    jsonStart();
    serverOut<<"{\"output\":\"";
    handleMenu(jsonNav);
    serverOut<<"\",\n\"menu\":";
    fromWeb=true;
    jsonNav.doOutput();
    fromWeb=false;
    serverOut<<"\n}";
    jsonEnd();
  });

  //file server, if MDNS handler not found.
  //so, do not put sensitive info in the data files
  // server.onNotFound([](){if (!handleFileRead(server.uri())) notfound();});

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Serving ArduinoMenu example.");
  #ifdef uTC_DEBUG
    server.serveStatic("/", SPIFFS, "/","max-age=30");//31536000");
  #else
    server.serveStatic("/", SPIFFS, "/","max-age=31536000");
  #endif
}

void loop(void){
  // serverOut.response.remove(0);
  wsOut.response.remove(0);//clear websocket json buffer
  webSocket.loop();
  server.handleClient();
  delay(1);
}
