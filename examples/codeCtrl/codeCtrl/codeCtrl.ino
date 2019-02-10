/* Arduino menu library example
Oct. 2016 Rui Azevedo (ruihfazevedo@gmail.com) www.r-site.net

Use the menu library with user code ctrl command

calling doNav with user command mode:

noCmd - clamp field values or do nothing
escCmd - exit
enterCmd - enter current option or validate field and exit
upCmd - move up or increment field value
downCmd - move down or decrement field value
leftCmd - move left or escape (not tested yet)
rightCmd - move right or enter (not tested yet)
idxCmd - enter option by index

this mode allows you to implement ANY input device

on this example only using

*/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/serialIO.h>
#include <menuIO/stringIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

#define LEDPIN 13
#define MAX_DEPTH 2
#define NAV_BTN 5
#define SEL_BTN 6

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

constText* constMEM textFilter MEMMODE=" .0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTWXYZ";
constText* constMEM textMask[] MEMMODE={textFilter};//this mask will repeat till the end of the field
char name[]="          ";//<-- menu will edit this text

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,EDIT("Name",name,textMask,doNothing,noEvent,noStyle)
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

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

stringIn<0> strIn;//buffer size: 2^5 = 32 bytes, eventually use 0 for a single byte
serialIn serial(Serial);
// use this commented lines if you want your stringIn object to be used as part or normal menu input
// menuIn* inputsList[]={&serial,&strIn};
// chainStream<sizeof(inputsList)> in(inputsList);
// NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

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
  pinMode(NAV_BTN,INPUT_PULLUP);
  pinMode(SEL_BTN,INPUT_PULLUP);
  Serial.println("menu 4.x");
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
  //if stringIn is a regular input then we should write to it here, before poll
  // strIn.write(...);//just put the character you want to send
  // nav.poll();//also do serial or stringIn input
  // or deal with charater input directly... (if you have your own input driver)
  if (Serial.available()) {
    //of course menu can read from Serial or even stringIn (se above how to use stringIn as a regular menu input)
    //but here we demonstrate the use of stringIn in direct call, by writing the data to stream and then call doInput with that stream
    if (strIn.write(Serial.read()))//so we just transfer data from serial to strIn
      nav.doInput(strIn);//and then let target parse input
  }
  nav.doOutput();//if not doing poll the we need to do output "manualy"
  digitalWrite(LEDPIN, ledCtrl);
}
