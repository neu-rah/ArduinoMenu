#include <Arduino.h>

/********************
Oct. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

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
#include <menuIO/ansiSerialOut.h>
#include <menuIO/serialIn.h>
// #include <streamFlow.h>
#include <AnsiStream.h>

using namespace Menu;
using namespace ANSI;

#ifdef DEBUG
Print& operator<<(Print&o, Menu::prompt&p) {
  print_P(o,p.getText());
  return o;
}
#endif

#define LEDPIN LED_BUILTIN

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint8_t> colors[6] MEMMODE={
  {{BLUE,WHITE}  ,{BLUE,WHITE,WHITE}},//bgColor
  {{BLACK,BLACK} ,{WHITE,BLUE,BLUE}},//fgColor
  {{BLACK,BLACK} ,{YELLOW,YELLOW,RED}},//valColor
  {{BLACK,BLACK} ,{WHITE,BLUE,YELLOW}},//unitColor
  {{BLACK,BLACK} ,{BLACK,BLUE,RED}},//cursorColor
  {{BLACK,BLACK}  ,{BLUE,RED,BLUE}},//titleColor
};

//define menu outputs ------------------------------------------------
#define MAX_DEPTH 4
// const panel panels[] MEMMODE={{1,1,16,10},{18,1,16,10},{36,1,16,10}};
// navNode* nodes[sizeof(panels)/sizeof(panel)];
// panelsList pList(panels,nodes,sizeof(panels)/sizeof(panel));
// idx_t ansi_tops[MAX_DEPTH];
// ansiSerialOut ansi(Serial,colors,ansi_tops,pList);//the output device, ansi-terminal Cols x Rows
// menuOut* constMEM outputs[] MEMMODE={&ansi};
// outputsList out(outputs,1);

MENU_OUTPUTS(out,MAX_DEPTH
  ,ANSISERIAL_OUT(Serial,colors,{1,1,26,10},{28,1,16,10},{46,1,16,10})
  ,NONE//must have 2 items at least
);

result doAlert(eventMask e, prompt &item);

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
    <<e<<" on ";
  #ifdef DEBUG
    Serial<<item;
  #endif
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial
    <<ANSI::xy(0,13)
    <<ANSI::setForegroundColor(WHITE)
    <<ANSI::setBackgroundColor(BLACK)
    <<e<<" event on ";
    #ifdef DEBUG
      Serial<<item;
    #endif
    Serial<<", proceed menu";
  Serial.flush();
  return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item);

int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
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
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);
  }
};

MENU(bigMenu,"BigMenu",doNothing,noEvent,noStyle
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
);

MENU(subSubMenu,"Sub-Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("SSub1",doNothing,noEvent)
  ,OP("SSub2",doNothing,noEvent)
  ,SUBMENU(bigMenu)
  ,EXIT("<Back")
);//just to test depth limit


MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"custom",showEvent,anyEvent)
  ,SUBMENU(subSubMenu)
  ,EXIT("<Back")
);

uint16_t year=2017;
uint16_t month=10;
uint16_t day=7;

//define a pad style menu (single line menu)
//here with a set of fields to enter a date in YYYY/MM/DD format
altMENU(menu,birthDate,"Birth",doNothing,noEvent,noStyle,(systemStyles)(_asPad|Menu::_menuData|Menu::_canNav|_parentDraw)
  ,FIELD(year,"","/",1900,3000,20,1,doNothing,noEvent,noStyle)
  ,FIELD(month,"","/",1,12,1,0,doNothing,noEvent,wrapStyle)
  ,FIELD(day,"","",1,31,1,0,doNothing,noEvent,wrapStyle)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(birthDate)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Show colors",showOutColors,enterEvent)
  ,OP("Show characters",showCharsMap,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);
////////////////////////////////////////////////////////////////////////
// menu navigation root object
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

result action2(eventMask e,navNode& nav, prompt &item) {
  Serial<<ANSI::xy(24,nav.sel+nav.root->showTitle);
  #ifdef DEBUG
    Serial<<item;
  #endif
  Serial<<" "<<e<<" event on ";
  #ifdef DEBUG
    Serial<<item;
  #endif
  Serial<<", quiting menu.";
  Serial.flush();
  return quit;
}

result alert(menuOut& o,idleEvent e) {
  if (e==idling)
    o <<ANSI::xy(0,0)
      <<ANSI::setBackgroundColor(BLACK)
      <<ANSI::setForegroundColor(WHITE);
    Serial<<"alert test"
      <<endl
      <<"press [select] to continue..."
      <<endl;
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

//when menu is suspended -----------------------------------------------
result idle(menuOut& o,idleEvent e) {
  switch(e) {
    //case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:
      Serial
        <<ANSI::xy(0,0)
        <<ANSI::setBackgroundColor(BLACK)
        <<ANSI::setForegroundColor(WHITE);
      Serial<<"suspended..."<<endl;
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
  Serial<<"menu 4.x test"<<endl;Serial.flush();
  //nav.idleTask=idle;//point a function to be used when menu is suspended
  //mainMenu[1].enabled=disabledStatus;
  //nav.showTitle=true;
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
