#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Streaming.h>

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

#define OK httpHeaders[ok]

class WiFiSessionOut:public esp8266Out {
public:
  WiFiSessionOut(
    const colorDef<esp8266Out::webColor> (&c)[nColors],
    idx_t* t,
    panelsList& p
  ):esp8266Out(c,t,p) {}
  result fmtStart(fmtParts part,navNode &nav,idx_t panelNr,idx_t idx=-1) override {
    switch(part) {
      case fmtPanel:*client<<"<div class=\"panel\">";break;
      case fmtTitle:*client<<"<h2 class=\"title\">";break;
      case fmtBody:*client<<"<ul class=\"ops\">";break;
      case fmtOp:*client<<"<li class=\"op\"><a href=\"/menu/"<<idx+1<<"\">";break;
      case fmtIdx:break;
      case fmtCursor:break;
      case fmtOpBody:break;
      case fmtPreview:break;
      default:break;
    }
    return proceed;
  }
  result fmtEnd(fmtParts part,navNode &nav,idx_t panelNr,idx_t idx=-1) override {
    switch(part) {
      case fmtPanel:*client<<"</div>";break;
      case fmtTitle:*client<<"</h2>";break;
      case fmtBody:*client<<"</ul>";break;
      case fmtOp:*client<<"</a></li>";break;
      case fmtIdx:break;
      case fmtCursor:break;
      case fmtOpBody:break;
      case fmtPreview:break;
      default:break;
    }
    return proceed;
  }
};

void action1(eventMask event, navNode& nav, prompt &item) {
  Serial<<"action1 called!"<<endl;
  nav.root->out[0]<<"This is action <b>1</b> web report"<<endl;
}
void action2(eventMask event, navNode& nav, prompt &item) {
  Serial<<"action2 called!"<<endl;
  nav.root->out[0]<<"This is action <b>2</b> web report"<<endl;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,enterEvent)
  ,OP("Op2",action2,enterEvent)
);

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

//serialOut outSerial(Serial);//the output device (just the serial port)
#define MAX_DEPTH 2
//PANELS(serial_pList,{0,0,40,10});
//navNode* serial_nodes[sizeof(serial_panels)/sizeof(panel)];
//panelsList serial_pList(serial_panels,serial_nodes,sizeof(serial_panels)/sizeof(panel));
//idx_t serial_tops[MAX_DEPTH];
//serialOut outSerial(Serial,serial_tops,serial_pList);//the output device (just the serial port)

idx_t tops[MAX_DEPTH];
PANELS(webPanels,{0,0,800,600});
WiFiSessionOut wifiOut(colors,tops,webPanels);
MENU_OUTLIST(out,&wifiOut);
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
    } else if (req.startsWith("GET /menu")) {
      Serial.println(req);
      wifiOut.client=&client;
      client.print(httpHeaders[ok]);
      client.print(httpHeaders[noCache]);
      client<<endl;
      client.print(httpHeaders[docHtml]);
      client<<"<html><head><title>ArduinoMenu library OTA</title></head><body>";
      uint8_t ch=req[10];
      //Serial<<"ch:"<<ch<<endl;
      if (ch>='0'&&ch<='9') {
        nav.doNav(navCmd(selCmd,ch));
        nav.doOutput();
        client<<"<hr/>";
        nav.doNav(navCmd(enterCmd));
        delay(1);
      } else nav.doOutput();
      client<<"</body></html>";
      wifiOut.client=NULL;
    } else client.print(httpHeaders[e404]);
  } else client.print(httpHeaders[e404]);
  delay(500);//this is not good, arbitrary delay not always enuf for the page... need a termination check
}
