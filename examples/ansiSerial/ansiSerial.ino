/********************
Oct. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu to ANSI serial terminal
output: ANSI Serial terminal
input: Serial

www.r-site.net
***/

#include <menu.h>
#include <dev/ansiSerialOut.h>

using namespace Menu;

#define LEDPIN A4

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial
    <<ANSI::xy(0,13)
    <<ANSI::setForegroundColor(WHITE)
    <<ANSI::setBackgroundColor(BLACK)
    <<ANSI::eraseLine()
    <<e<<" on "<<item;
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial<<ANSI::xy(22,1+nav.root->showTitle)
    <<e<<" event on "<<item<<", proceed menu";
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<ANSI::xy(22,nav.sel+nav.root->showTitle)
    <<item<<" "<<e<<" event on "<<item<<", quiting menu.";
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
  void printTo(idx_t i,navNode &nav,menuOut& out) override {
    out<<"special prompt!";
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,EXIT("<Back")
);

// each color is in the format {{normal disabled,normal enabled},{selected disabled,selected enabled}}
// monochromatic color table
const uint8_t colors[][2][2] MEMMODE={
  {{BLUE,BLUE},{WHITE,WHITE}},//bgColor
  {{BLACK,WHITE},{BLACK,BLUE}},//fgColor
  {{WHITE,YELLOW},{BLACK,RED}},//valColor
  {{WHITE,WHITE},{BLACK,BLACK}},//unitColor
  {{BLACK,BLACK},{WHITE,WHITE}},//cursorColor
  {{BLUE,BLUE},{RED,RED}},//titleColor
};

ansiSerialOut ansi(Serial,colors,20,8);//the output device, ansi-terminal Cols x Rows
menuOut* outputs[]={&ansi};
outputsList out(outputs,1);
NAVROOT(nav,mainMenu,2,Serial,out);

//when menu is suspended
result idle(idleEvent e) {
  switch(e) {
    case idleStart:Serial<<"suspending menu!"<<endl;break;
    case idling:Serial<<"suspended..."<<endl;break;
    case idleEnd:Serial<<"resuming menu."<<endl;break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  options.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
