/* Arduino menu library example
Oct. 2016 Rui Azevedo (ruihfazevedo@gmail.com) www.r-site.net

Use the menu library with user code ctrl command

calling doNav with user command mode:

noCmd - clamp field values or do nothing
escCmd - exit
enterCmd - enter current option or validate field and exit
upCmd - move up or increment field value
downCmd - move down or decrement field value
leftCmd - move left or escape
rightCmd - move right or enter
idxCmd - enetr option by index

this mode allows you to implement ANY input device

on this example only using

*/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>

using namespace Menu;

#define LEDPIN 13
#define MAX_DEPTH 2
#define NAV_BTN 5
#define SEL_BTN 6

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial<<e<<" event on "<<item<<", proceed menu"<<endl;
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<item<<" "<<e<<" event on "<<item<<", quiting menu."<<endl;
  Serial.flush();
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
  altPrompt(const promptShadow& p):prompt(p) {}
  idx_t printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len) override {
    return out.printRaw("special prompt!",len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,anyEvent,wrapStyle
  ,OP("Sub1",showEvent,enterEvent)
  ,OP("Sub2",showEvent,enterEvent)
  ,OP("Sub3",showEvent,enterEvent)
  ,altOP(altPrompt,"",showEvent,enterEvent)
  ,EXIT("<Back")
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o<<"alert test";
    o.setCursor(0,1);
    o<<"[select] to continue...";
  }
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
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

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(LEDPIN, OUTPUT);
  pinMode(NAV_BTN,INPUT_PULLUP);
  pinMode(SEL_BTN,INPUT_PULLUP);
}

#define SOFT_DEBOUNCE_MS 100

void loop() {
  if (!digitalRead(SEL_BTN)) {
    delay(SOFT_DEBOUNCE_MS);
    while(!digitalRead(SEL_BTN));
    nav.doNav(enterCmd);
    delay(SOFT_DEBOUNCE_MS);
  }
  if (!digitalRead(NAV_BTN)) {
    delay(SOFT_DEBOUNCE_MS);
    while(!digitalRead(NAV_BTN));//wait for button release
    nav.doNav(upCmd);
    delay(SOFT_DEBOUNCE_MS);
  }
  nav.poll();//also do serial input
  //nav.doOutput();
  digitalWrite(LEDPIN, ledCtrl);
}
