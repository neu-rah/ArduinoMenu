/********************
Arduino generic menu system
U8GLib menu example
U8Glib: https://github.com/olikraus/U8glib_Arduino

Jul.2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes
*/

#include <menu.h>
#include <dev/serialOut.h>
#include <dev/U8GLibOut.h>

using namespace Menu;

#define LEDPIN A4

#define U8_DC 9
#define U8_CS 8
#define U8_RST 7

U8GLIB_PCD8544 u8g(U8_CS, U8_DC, U8_RST) ;

result zZz() {Serial<<"zZz"<<endl;return proceed;}

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
CHOOSE(chooseTest,chooseMenu,"Choose ",doNothing,noEvent,noStyle
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

MENU(mainMenu,"Main menu",zZz,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
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
  {{0,0},{0,1}},//bgColor
  {{1,1},{1,0}},//fgColor
  {{1,0},{1,0}},//valColor <--- BUG: value select meaning edit makes bad combination with normal selection
  {{1,1},{1,0}},//unitColor
  {{0,0},{1,1}},//cursorColor
  {{0,0},{1,1}},//titleColor
};

serialOut outSerial(Serial);//the output device (just the serial port)
menuU8G gfx(u8g,colors,5,12);
menuOut* outputs[]={&gfx,&outSerial};
outputsList out(outputs,2);
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
  u8g.setFont(u8g_font_helvR08);
  u8g.firstPage();
  do {
    gfx.setCursor(0,0);
    gfx.print("Menu 3.x test");
    gfx.setCursor(0,1);
    gfx.print("on U8Glib");
  } while(u8g.nextPage());
  delay(2000);
}

void loop() {
  u8g.firstPage();
  do {
    nav.poll();
    digitalWrite(LEDPIN, ledCtrl);
  } while( u8g.nextPage() );
  delay(300);//simulate a delay when other tasks are done
}
