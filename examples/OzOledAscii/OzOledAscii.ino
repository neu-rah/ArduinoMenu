
//using https://github.com/kdkanishka/ozoled-oled-display-096-i2c-driver-library

#include <Arduino.h>
#include "OzOLED.h"

#include <menu.h>
#include <menuIO/OzOledAsciiOut.h>
#include <menuIO/serialIn.h>
using namespace Menu;

//Redefine already existing object
#define oled OzOled

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test=55;

result action1(eventMask e) {
  Serial.print(e);
  Serial.println(" action1 executed, proceed menu");Serial.flush();
  return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item) {
  Serial.print(e);
  Serial.println(" action2 executed, quiting menu");
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
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

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
//Only one build-in font 
#define fontW 8
#define fontH 8

//describing a menu output device without macros
//define at least one panel for menu output
const panel panels[] MEMMODE={{0,0,128/fontW,64/fontH}};
navNode* nodes[sizeof(panels)/sizeof(panel)];//navNodes to store navigation status
panelsList pList(panels,nodes,1);//a list of panels and nodes
idx_t tops[MAX_DEPTH]={0,0};//store cursor positions for each level
OzOledAsciiOut outOLED(&oled,tops,pList);//oled output device menu driver
menuOut* constMEM outputs[] MEMMODE={&outOLED};//list of output devices
outputsList out(outputs,1);//outputs list

//macro to create navigation control root object (nav) using mainMenu
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print("alert test");
    o.setCursor(0,1);
    o.print("press [select]");
    o.setCursor(0,2);
    o.print("to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

//when menu is suspended
result idle(menuOut &o, idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");break;
  }
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");Serial.flush();
  oled.init();
  oled.clearDisplay();
  //Vertical flip, comment out if display upside down.
  oled.sendCommand(0xC8);
  oled.sendCommand(0xA1);
  //End Vertical flip
  oled.printString("Hello world!");
  nav.idleTask=idle;//point a function to be used when menu is suspended
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
