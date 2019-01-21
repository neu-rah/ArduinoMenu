/* Arduino menu library example
Oct. 2016 Rui Azevedo (ruihfazevedo@gmail.com) www.r-site.net

Digital keypad (3 buttons) using the menu keyIn driver
*/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>
#include <menuIO/altKeyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

#define LEDPIN 13
#define MAX_DEPTH 2

#define BTN_SEL 6	// Select button
#define BTN_UP 7 // Up
#define BTN_DOWN 8 // Down

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial.print("action1 event:");
  Serial.println(e);
  Serial.flush();
  return proceed;
}

result action2(eventMask e) {
  Serial.print("actikon2 event:");
  Serial.println(e);
  Serial.flush();
  return quit;
}

int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,wrapStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,wrapStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,wrapStyle
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
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,anyEvent,wrapStyle
  ,OP("Sub1",showEvent,enterEvent)
  ,OP("Sub2",showEvent,enterEvent)
  ,OP("Sub3",showEvent,enterEvent)
  ,altOP(altPrompt,"",showEvent,enterEvent)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

keyMap joystickBtn_map[]={
 {-BTN_SEL, defaultNavCodes[enterCmd].ch,INPUT_PULLUP} ,
 {-BTN_UP, defaultNavCodes[upCmd].ch,INPUT_PULLUP} ,
 {-BTN_DOWN, defaultNavCodes[downCmd].ch,INPUT_PULLUP}  ,
};
keyIn<3> joystickBtns(joystickBtn_map);

serialIn serial(Serial);
menuIn* inputsList[]={&joystickBtns,&serial};
chainStream<2> in(inputsList);//3 is the number of inputs

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print("alert test");
    o.setCursor(0,1);
    o.print("[select] to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(LEDPIN, OUTPUT);
  joystickBtns.begin();
}

#define SOFT_DEBOUNCE_MS 100

void loop() {
  nav.poll();//also do serial input
  digitalWrite(LEDPIN, ledCtrl);
}
