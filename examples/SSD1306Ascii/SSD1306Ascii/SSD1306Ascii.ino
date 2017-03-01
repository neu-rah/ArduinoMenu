#include <Arduino.h>

#define I2C_ADDRESS 0x3C
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
using namespace Menu;

SSD1306AsciiWire oled;

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;

result action1(eventMask e) {
  Serial<<e<<" action1 executed, proceed menu"<<endl;Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<item<<" "<<e<<" action2 executed, quiting menu"<<endl;
  return quit;
}

int ledCtrl=LOW;

result ledOn() {
  ledCtrl=HIGH;
  return proceed;
}
result ledOff() {
  ledCtrl=LOW;
  return proceed;
}

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

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(const promptShadow& p):prompt(p) {}
  idx_t printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len) override {
    return out.printRaw("special prompt!",len);
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

result alert(menuOut& o,idleEvent e) {
  //if (e==idling)
    o<<"alert test"<<endl<<"press [select]"<<endl<<"to continue..."<<endl;
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

#define fontW 5
#define fontH 7

//describing a menu output device without macros
//define at least one panel for menu output
const panel panels[] MEMMODE={{0,0,128/fontW,64/fontH}};
navNode* nodes[sizeof(panels)/sizeof(panel)];//navNodes to store navigation status
panelsList pList(panels,nodes,1);//a list of panels and nodes
idx_t tops[MAX_DEPTH];//store cursor positions for each level
SSD1306AsciiOut outOLED(&oled,tops,pList);//oled output device menu driver
menuOut* outputs[]={&outOLED};//list of output devices
outputsList out(outputs,1);//outputs list

//macro to create navigation control root object (nav) using mainMenu
NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//when menu is suspended
result idle(menuOut &o, idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:o<<"suspended..."<<endl;break;
    case idleEnd:o<<"resuming menu."<<endl;break;
  }
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!");
  nav.idleTask=idle;//point a function to be used when menu is suspended
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
