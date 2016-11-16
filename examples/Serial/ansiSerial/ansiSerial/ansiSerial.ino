#include <Arduino.h>

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

status: BUGGY!

www.r-site.net
***/
#include <menu.h>
#include <menuIO/ansiSerialOut.h>

using namespace Menu;

#ifdef ARDUINO_SAM_DUE
  #define LEDPIN 13
#else
  #define LEDPIN A3
#endif

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
#define MAX_DEPTH 2
/*const panel panels[] MEMMODE={{1,1,16,10},{18,1,16,10},{36,1,16,10}};
navNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,sizeof(panels)/sizeof(panel));
idx_t ansi_tops[MAX_DEPTH];
ansiSerialOut ansi(Serial,colors,ansi_tops,pList);//the output device, ansi-terminal Cols x Rows
menuOut* outputs[]={&ansi};
outputsList out(outputs,1);*/

MENU_OUTPUTS(out,MAX_DEPTH
  ,ANSISERIAL_OUT(Serial,colors,{1,1,16,10})//,{18,1,16,10},{36,1,16,10})
  ,NONE//must have 2 items at least
);

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
    out.setCursor(x,y-1);
    out<<"XX";
}

void showColorDef(menuOut& out,colorDefs def,int x,int y) {
  out<<ANSI::setBackgroundColor(BLACK)<<ANSI::setForegroundColor(WHITE);
  out<<ANSI::xy(x+1,y+1)<<def<<ANSI::xy(x+16,y+1)<<"Disabled | Enabled | Editing  "<<endl;
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

result showColors(menuOut& o,idleEvent e) {
  switch(e) {
    case idling: for(int c=0;c<nColors;c++) showColorDef(o,(colorDefs)c,0,c<<2); break;
    default:
      o<<ANSI::xy(80,25)<<ANSI::eraseScreen()<<ANSI::xy(1,1);
      break;
  }
  return proceed;
}

result showChars(menuOut& o,idleEvent e) {
  switch(e) {
    case idling:
      //o<<"len:"<<strlen(box_top);
      o<<F("╭─┬╮")<<endl;
      o<<F("│ ││")<<endl;
      o<<F("├─┼┤")<<endl;
      o<<F("╰─┴╯")<<endl;
      break;
    default:
      o<<ANSI::setForegroundColor(WHITE)
        <<ANSI::setBackgroundColor(BLACK)
        <<ANSI::xy(80,25)
        <<ANSI::eraseScreen()
        <<ANSI::xy(1,1);
      break;
  }
  return proceed;
}

void showCharsMap(eventMask event, navNode& nav) {nav.root->idleOn(showChars);}

void showOutColors(eventMask event, navNode& nav) {nav.root->idleOn(showColors);}

//menu definition ------------------------------------
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
  Serial
    <<ANSI::xy(0,13)
    <<ANSI::setForegroundColor(WHITE)
    <<ANSI::setBackgroundColor(BLACK)
    <<e<<" event on "<<item<<", proceed menu";
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<ANSI::xy(24,nav.sel+nav.root->showTitle)
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
  idx_t printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len) override {
    return out.printRaw("special prompt!",len);
  }
};

MENU(subSubMenu,"Sub-Sub-Menu",doNothing,noEvent,noStyle
  ,OP("SSub1",doNothing,noEvent)
  ,OP("SSub2",doNothing,noEvent)
  ,EXIT("<Back")
);//just to test depth limit

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"custom",showEvent,anyEvent)
  ,SUBMENU(subSubMenu)
  ,EXIT("<Back")
);

extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling)
    o
      <<ANSI::xy(0,0)
      <<ANSI::setBackgroundColor(BLACK)
      <<ANSI::setForegroundColor(WHITE)
      <<"alert test"
      <<endl
      <<"press [select] to continue..."
      <<endl;
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
  ,OP("Show colors",showOutColors,enterEvent)
  ,OP("Show characters",showCharsMap,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);
////////////////////////////////////////////////////////////////////////
// menu navigation root object
NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//when menu is suspended -----------------------------------------------
result idle(menuOut& o,idleEvent e) {
  switch(e) {
    //case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:
      o
      <<ANSI::xy(0,0)
      <<ANSI::setBackgroundColor(BLACK)
      <<ANSI::setForegroundColor(WHITE)
      <<"suspended..."<<endl;
      break;
    default: break;
    //case idleEnd:o<<"resuming menu."<<endl;break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  /*while(!Serial.available()) {Serial<<".";delay(300);}
  while(Serial.available()) Serial.read();*/
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
  nav.showTitle=true;
  //nav.printMenu(1);
  //ansi.fill(1, 1, 2, 2, 'X');
  //Serial<<"pList[0]:{"<<pList[0].x<<","<<pList[0].y<<","<<pList[0].w<<","<<pList[0].h<<"}"<<endl;
  delay(1000);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
