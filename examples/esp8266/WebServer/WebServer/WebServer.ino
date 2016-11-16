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

#define MAX_DEPTH 2
idx_t tops[MAX_DEPTH];
PANELS(webPanels,{0,0,30,20});

htmlFmt<esp8266_WebServerOut> serverOut(server,/*colors,*/tops,webPanels);

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

//the menu
int timeOn;
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
  ,FIELD(timeOn,"On","ms",0,500,100,10, doNothing, noEvent, noStyle)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
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
    <<"<!DOCTYPE html>\r\n<html><head profile=\"http://www.w3.org/2005/10/profile\">\r\n"
    <<"<meta charset=\"utf-8\">\r\n"
    <<"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n"
    <<"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n"
    <<"<title>ArduinoMenu library OTA</title>\r\n"
    <<"<link rel=\"icon\" type=\"image/png\" href=\"/logo.png\">\r\n"
    <<"<script src=\"https://code.jquery.com/jquery-3.1.1.slim.min.js\" integrity=\"sha256-/SIrNqv8h6QGKDuNoLGA4iret+kyesCkHGzVUUV0shc=\" crossorigin=\"anonymous\"></script>\r\n"
    <<"<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">\r\n"
    <<"<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>\r\n"
    <<"<!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->\r\n"
    <<"<!--[if lt IE 9]>\r\n"
    <<"  <script src=\"https://oss.maxcdn.com/html5shiv/3.7.3/html5shiv.min.js\"></script>\r\n"
    <<"  <script src=\"https://oss.maxcdn.com/respond/1.4.2/respond.min.js\"></script>\r\n"
    <<"<![endif]-->\r\n"
    <<"<script type=\"text/javascript\" src=\"r-site.js\" ></script>\r\n"
    <<"<link rel=\"stylesheet\" type=\"text/css\" href=\"/index.css\">\r\n"
    <<"<link rel=\"stylesheet\" type=\"text/css\" href=\"/r-site.css\">\r\n"
    <<"<script>$(function() {init();});</script>\r\n"
    <<"</head><body class=\"ArduinoMenu\">\r\n"
    <<"<div class=\"site-wrapper\">\r\n"
    <<"  <div class=\"site-wrapper-inner\">\r\n"
    <<"    <div class=\"cover-container\">\r\n"
    <<"      <div class=\"masthead clearfix\">\r\n"
    <<"        <div class=\"inner\">\r\n"
    <<"          <h3 class=\"masthead-brand\">ArduinoMenu library</h3>\r\n"
    <<"          <nav>\r\n"
    <<"            <ul class=\"nav masthead-nav\">\r\n"
    <<"              <li class=\"active\"><a href=\"https://github.com/neu-rah/ArduinoMenu\">Home</a></li>\r\n"
    <<"              <!--li><a href=\"#\">Features</a></li-->\r\n"
    <<"              <li><a href=\"http://www.r-site.net\">Contact</a></li>\r\n"
    <<"            </ul>\r\n"
    <<"          </nav>\r\n"
    <<"        </div>\r\n"
    <<"      </div>\r\n"
    <<"   <div class=\"inner cover\">\r\n";
}

void pageEnd() {
  serverOut
      <<"</div><div class=\"mastfoot\"><div class=\"inner\">\r\n"
      <<"<p><a href=\"/\">back to control page</a> ArduinoMenu library <a href=\"https://github.com/neu-rah/ArduinoMenu\">https://github.com/neu-rah/ArduinoMenu</a>.</p>\r\n"
      <<"</div></div></div></div></div></body></html>";
  server.send(200, "text/html", serverOut.response);
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

bool handleFileRead(String path){
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial<<"file not found "<<path<<endl;
  return false;
}

inline void notfound() {server.send(404, "text/plain", "FileNotFound");}

bool handleMenu(){
  if (server.hasArg("at"))
    return nav.async(server.arg("at").c_str());
  else return true;
}

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

  server.on("/", HTTP_GET, [](){
    handleMenu();//no output will be done on this mode
    if(!handleFileRead("/index.html")) notfound();}//static page
  );

  server.on("/logo.png", HTTP_GET, [](){
    if(!handleFileRead("/logo.png")) notfound();
  });
  server.on("/r-site.css", HTTP_GET, [](){
    if(!handleFileRead("/r-site.css")) notfound();
  });
  server.on("/r-site.js", HTTP_GET, [](){
    if(!handleFileRead("/r-site.js")) notfound();
  });

  server.on("/index.css", HTTP_GET, [](){
    if(!handleFileRead("/index.css")) notfound();
  });

  server.on("/menu", HTTP_GET, []() {
    handleMenu();
    String r(serverOut.response);
    serverOut.response.remove(0);
    pageStart();
    nav.doOutput();
    serverOut<<"<hr/>";
    serverOut<<r;
    pageEnd();
    delay(1);
  });

  server.onNotFound(notfound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Serving ArduinoMenu example.");
}

void loop(void){
  serverOut.response.remove(0);
  server.handleClient();
  delay(1);
}
