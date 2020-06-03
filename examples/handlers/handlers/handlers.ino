/********************
Arduino generic menu system
printing events and navigation info

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

void showPath(navRoot& root) {
  Serial.print("nav level:");
  Serial.print(root.level);
  Serial.print(" path:[");
  for(int n=0;n<=root.level;n++) {
    Serial.print(n?",":"");
    Serial.print(root.path[n].sel);
  }
  Serial.println("]");
}

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.println();
  Serial.println("========");
  Serial.print("Event for target: 0x");
  Serial.println((long)nav.target,HEX);
  showPath(*nav.root);
  Serial.print(e);
  switch(e) {
    case noEvent://just ignore all stuff
      Serial.println(" noEvent");break;
    case activateEvent://this item is about to be active (system event)
      Serial.println(" activateEvent");break;
    case enterEvent://entering navigation level (this menu is now active)
      Serial.println(" enterEvent");break;
    case exitEvent://leaving navigation level
      Serial.println(" exitEvent");break;
    case returnEvent://TODO:entering previous level (return)
      Serial.println(" returnEvent");break;
    case focusEvent://element just gained focus
      Serial.println(" focusEvent");break;
    case blurEvent://element about to lose focus
      Serial.println(" blurEvent");break;
    case selFocusEvent://TODO:child just gained focus
      Serial.println(" selFocusEvent");break;
    case selBlurEvent://TODO:child about to lose focus
      Serial.println(" selBlurEvent");break;
    case updateEvent://Field value has been updated
      Serial.println(" updateEvent");break;
    case anyEvent:
      Serial.println(" anyEvent");break;
  }
  return proceed;
}

float test=55;

int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

TOGGLE(ledCtrl,setLed,"Led: ",showEvent,anyEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,showEvent,anyEvent)
  ,VALUE("Off",LOW,showEvent,anyEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",showEvent,anyEvent,noStyle
  ,VALUE("Zero",0,showEvent,anyEvent)
  ,VALUE("One",1,showEvent,anyEvent)
  ,VALUE("Two",2,showEvent,anyEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",showEvent,anyEvent,noStyle
  ,VALUE("First",1,showEvent,anyEvent)
  ,VALUE("Second",2,showEvent,anyEvent)
  ,VALUE("Third",3,showEvent,anyEvent)
  ,VALUE("Last",-1,showEvent,anyEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  // altPrompt(constMEM promptShadow& p):prompt(p) {}
  using prompt::prompt;
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

uint16_t year=2017;
uint16_t month=10;
uint16_t day=7;

//define a pad style menu (single line menu)
//here with a set of fields to enter a date in YYYY/MM/DD format
//altMENU(menu,birthDate,"Birth",showEvent,anyEvent,noStyle,(systemStyles)(_asPad|Menu::_menuData|Menu::_canNav|_parentDraw)
PADMENU(birthDate,"Birth",showEvent,anyEvent,noStyle
  ,FIELD(year,"","/",1900,3000,20,1,showEvent,anyEvent,noStyle)
  ,FIELD(month,"","/",1,12,1,0,showEvent,anyEvent,wrapStyle)
  ,FIELD(day,"","",1,31,1,0,showEvent,anyEvent,wrapStyle)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

const char* constMEM alphaNum MEMMODE=" 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
const char* constMEM alphaNumMask[] MEMMODE={alphaNum};
char name[]="                                                  ";

MENU(mainMenu,"Main menu",showEvent,anyEvent,wrapStyle
  ,OP("Op1",showEvent,anyEvent)
  ,OP("Op2",showEvent,anyEvent)
  ,altFIELD(decPlaces<3>::menuField,test,"Test","%",0,100,1,0.001,showEvent,anyEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Name",name,alphaNumMask,showEvent,anyEvent,noStyle)
  ,EDIT("Hex",buf1,hexNr,showEvent,anyEvent,noStyle)
  ,SUBMENU(birthDate)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

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

result idle(menuOut &o, idleEvent e) {
  // o.clear();
  switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");
      nav.reset();
      break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,ledCtrl);
  delay(500);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");Serial.flush();
  nav.timeOut=120;
  nav.idleTask=idle;//point a function to be used when menu is suspended
  // nav.idleOn();//this menu will start on idle state, press select to enter menu
  //nav.doInput("323");
  // nav.useAccel=false;
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
