/********************
Arduino generic menu system
WebServer menu example
WebServer: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer

Nov. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 4.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu on web browser served by esp8266 device
output: Web browser
input: Web browser

*/
#include <menu.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/htmlFmt.h>
#include <FS.h>

using namespace Menu;

const char* ssid = "r-site.net";
const char* password = "rsite.2011";

//define server and menu output to server
//some IO definitions/options
#define BLACK "#000"
#define BLUE "navy"
#define GRAY "#888"
#define WHITE "white"
#define RED "red"
#define YELLOW "gold"

ESP8266WebServer server(80);
#define DBG_OUTPUT_PORT Serial

//not using colors yet
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
PANELS(webPanels,{0,0,30,20});

htmlFmt<esp8266_WebServerOut> serverOut(server,colors,tops,webPanels);

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
int timeOn;
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
  ,FIELD(timeOn,"On","ms",0,500,100,10, doNothing, noEvent, noStyle)
);

result idle(menuOut& o,idleEvent e) {
  //if (e==idling)
  Serial.println("suspended");
  o<<"suspended..."<<endl<<"press [select]"<<endl<<"to continue"<<endl<<(millis()%1000);
  return quit;
}

MENU_OUTLIST(out,&serverOut);
NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

config myOptions={'*','-',false,false,defaultNavCodes};


//const int led = 13;
void pageStart() {
  server.sendHeader("Cache-Control","no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma","no-cache");
  server.sendHeader("Expires","0");
  serverOut
    <<"<!DOCTYPE html>\r\n<html><head profile=\"http://www.w3.org/2005/10/profile\">"
    <<"<title>ArduinoMenu library OTA</title>"
    <<"<link rel=\"icon\" type=\"image/png\" href=\"/logo.png\">"
    <<"<script type=\"text/javascript\" src=\"r-site.js\" ></script>"
    <<"<link rel=\"stylesheet\" type=\"text/css\" href=\"/r-site.css\"></head><body class=\"ArduinoMenu\">";
}

void pageEnd() {
  serverOut<<"</body></html>";
  server.send(200, "text/html", serverOut.response);
}

void handleRoot() {
  pageStart();
  nav.doOutput();
  pageEnd();
}

//old parsing
void menuParser() {
  String uri=server.uri();
  if (uri.startsWith("/")) {
    uint8_t ch=uri[1];
    if (ch>='0'&&ch<='9') {
      nav.doNav(navCmd(idxCmd,ch));
      String r(serverOut.response);
      serverOut.response.remove(0);
      pageStart();
      nav.doOutput();
      serverOut<<"<hr/>";
      serverOut<<r;
      //nav.doNav(navCmd(enterCmd));
      pageEnd();
      delay(1);
    } else {
      pageStart();
      nav.doOutput();
      pageEnd();
    }
  } else handleNotFound();
}

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
  //digitalWrite(led, 0);
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

template<typename T> HardwareSerial& operator<<(HardwareSerial& o,T t) {o.print(t);return o;}

bool handleFileRead(String path){
  //DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
      //Serial<<"file exists "<<path<<endl;
    File file = SPIFFS.open(path, "r");
    //Serial<<"file opened"<<endl;
    size_t sent = server.streamFile(file, contentType);
    //Serial<<"sent "<<sent<<" bytes"<<endl;
    file.close();
    return true;
  }
  Serial<<"file not found "<<path<<endl;
  return false;
}

#include "logo.h"
void logo() {
  server.setContentLength(sizeof(logoPng));
  server.sendHeader("Content-type", "image/png");
  server.sendContent_P(logoPng, sizeof(logoPng));
}

inline void notfound() {server.send(404, "text/plain", "FileNotFound");}

void setup(void){
  options=&myOptions;
  Serial.begin(115200);
  Serial.setDebugOutput(1);
  Serial.setDebugOutput(0);
  while(!Serial);
  delay(10);
  Serial.println("");
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
  Serial.println("");
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  nav.idleTask=idle;//point a function to be used when menu is suspended

  server.on("/favicon.ico",handleNotFound);
  //server.on("/logo.png",logo);

  server.on("/", handleRoot);

  server.on("/logo.png", HTTP_GET, [](){
    if(!handleFileRead("/logo.png")) notfound();
  });
  server.on("/r-site.css", HTTP_GET, [](){
    if(!handleFileRead("/r-site.css")) notfound();
  });
  server.on("/r-site.js", HTTP_GET, [](){
    if(!handleFileRead("/r-site.js")) notfound();
  });

  server.on("/menu", HTTP_GET, [](){
    //if (!server.uri().startsWith("/menu")) return;
    Serial<<"/menu parser "<<server.args()<<" args"<<endl;
    if (server.hasArg("at")) {// at selector
      String at=server.arg("at");
      Serial<<"at:"<<at<<endl;
      prompt* p=mainMenu.async(at.c_str());
      if (p) {
        //execute command or enter
        navCmds cmd=server.hasArg("cmd")?getCmd(server.arg("cmd")):enterCmd;
        if (cmd==noCmd) cmd=navCmd;
        Serial<<"cmd:"<<cmd<<endl;
        char key=(server.hasArg("key")?server.arg("key"):"").c_str()[0];
        if (cmd==enterCmd) p->doNav(navCmd(cmd,key));<-- missing navNode object!
        p() <-- just do blind enter, need increment/decrement and edit status.
        how to do both things without navNode
        navNode is required for upCmd, downCmd, escCmd1


        String r(serverOut.response);
        serverOut.response.remove(0);
        pageStart();//TODO, make this accept serverOut param and write to it avoiding this weird copy.. well we still need the copy to print processed menu first
        nav.doOutput();
        serverOut<<"<hr/>";
        serverOut<<r;
        //nav.doNav(navCmd(enterCmd));
        pageEnd();
        delay(1);

        pageStart();
        serverOut<<p->getText();//just gbecause eps8266 uses ram! (no PROGMEM  here)
        pageEnd();
      } else notfound();
    }
  });
  /*server.on("/r-site.css", []() {server.send(200, "text/css",
    "body {background-color:#132; color:#8a9;}\r\n"
    "#panel a {color: #acb; text-decoration: none;}\r\n"
    "#panel ul {list-style-type: none;}\r\n"
  );});*/

  /*server.on("/r-site.js", []() {server.send(200, "text/javascript",
    "alert('scripts loaded');"
    "function updateField() {"
    "}"
    "function init() {"
    "  var f=document.querySelector(\"input .field\");"
    "  for(o in f) console.log(o);"
    "}"
    "window.onload = init();"
  );});*/

  server.onNotFound(notfound);
  //server.onNotFound(menuParser);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Serving ArduinoMenu example.");
}

void loop(void){
  serverOut.response.remove(0);
  server.handleClient();
  delay(1);
}
