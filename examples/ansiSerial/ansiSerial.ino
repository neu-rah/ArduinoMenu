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

draw color menu on ansi serial device
to see the result on linux machines use:

screen /dev/ttyUSBn 115200

replace the port (/dev/ttyUSBn) with your appropriate port
screen utility exits with [Ctrl+A \ y]

www.r-site.net
***/

#include <menu.h>
#include <dev/ansiSerialOut.h>

using namespace Menu;

#define LEDPIN A4

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint8_t> colors[] MEMMODE={
  {{BLUE,WHITE}  ,{BLUE,WHITE,WHITE}},//bgColor
  {{BLACK,BLACK} ,{WHITE,BLUE,BLUE}},//fgColor
  {{BLACK,BLACK} ,{YELLOW,YELLOW,RED}},//valColor
  {{BLACK,BLACK} ,{WHITE,BLUE,YELLOW}},//unitColor
  {{BLACK,BLACK} ,{BLACK,BLUE,RED}},//cursorColor
  {{BLACK,BLACK}  ,{BLUE,RED,BLUE}},//titleColor
};

//define menu outputs ------------------------------------------------
ansiSerialOut ansi(Serial,colors,20,8);//the output device, ansi-terminal Cols x Rows
menuOut* outputs[]={&ansi};
outputsList out(outputs,1);

// menu aux functions and vars ------------------------------------------------
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

void putColor(
  menuOut& out,
  colorDefs def,
  bool selected,
  status stat,
  bool edit
  ,int x,int y
) {
    out.fill(x,y,x+8,y,' ',bgColor,selected,stat,edit);
    out.setColor(def,selected,stat,edit);
    out.setCursor(x,y);
    out<<"XX";
}

void showColorDef(menuOut& out,colorDefs def,int x,int y) {
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+1)<<def<<ANSI::xy(x+16,y+1)<<"Disabled | Enabled | Editing  "<<ANSI::reset()<<endl;
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+2)<<"normal";
  putColor(out, def, false, disabledStatus, false,x+15,y+1);
  putColor(out, def, false, enabledStatus, false,x+25,y+1);
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+3)<<"selected";
  putColor(out, def, true, disabledStatus, false,x+15,y+2);
  putColor(out, def, true, enabledStatus, false,x+25,y+2);
  putColor(out, def, true, enabledStatus, true,x+35,y+2);
}

void showColors(menuOut& o) {
  out.clear();
  for(int c=0;c<nColors;c++) showColorDef(o,c,0,c<<2);
}

void showOutColors();
////////////////////////////////////////////////////////////////////////////////
// menu structure
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

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o<<"alert test"<<endl<<"press [select] to continue..."<<endl;
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
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Show colors",showOutColors,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

////////////////////////////////////////////////////////////////////////
// menu navigation root object
NAVROOT(nav,mainMenu,2,Serial,out);

//aux functions that uses nav
void showOutColors() {
  nav.idleOn();
  showColors(ansi);
}

//when menu is suspended -----------------------------------------------
result idle(menuOut& o,idleEvent e) {
  switch(e) {
    case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:o<<"suspended..."<<endl;break;
    case idleEnd:o<<"resuming menu."<<endl;break;
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
