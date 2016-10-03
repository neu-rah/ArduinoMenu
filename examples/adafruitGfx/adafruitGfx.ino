/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu with adafruit GFX
output: Nokia 5110 display (PDC8544 HW SPI)
input: Serial
www.r-site.net
***/

#include <menu.h>
#include <SPI.h>//this display uses hardware SPI
#include <Adafruit_GFX.h>
//#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_PCD8544.h>
#include <dev/adafruitGfxOut.h>
#include <dev/serialOut.h>
//#include <FreeMono9pt7b.h>

using namespace Menu;

#define LEDPIN A4

#define GFX_DC 9
#define GFX_CS 8
#define GFX_RST 7

//Adafruit_ST7735 gfx(GFX_CS, GFX_DC, GFX_RST);
//Adafruit_PCD8544 gfx(GFX_DC,GFX_CS,GFX_RST);
Adafruit_PCD8544 nokia(GFX_DC,GFX_CS,GFX_RST);

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

const uint16_t colors[][2][2] MEMMODE={
  {{0,1},{0,1}},//option color
  {{0,1},{0,1}},//selected option color
  {{0,1},{0,1}},//menu color
  {{0,1},{0,1}},//selected menu color
  {{0,1},{0,1}},//fieldColor
  {{0,1},{0,1}},//fieldColorHi
  {{0,1},{0,1}},//valueColor
  {{0,1},{0,1}},//valueColorHi
  {{0,1},{0,1}},//unitColor
  {{0,1},{0,1}},//unitColorHi
  {{0,1},{0,1}}//cursorColor
};

serialOut outSerial(Serial);//the output device (just the serial port)
menuGFX outGFX(nokia,colors);//output device for LCD
menuOut* outputs[]={&outGFX,&outSerial};
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
  digitalWrite(LEDPIN,HIGH);
  delay(500);
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  options.idleTask=idle;//point a function to be used when menu is suspended

  SPI.begin();
  nokia.begin();
  nokia.clearDisplay();
  nokia.print("Menu test on GFX");
  nokia.setContrast(50);
  nokia.display(); // show splashscreen
  delay(500);
  nokia.clearDisplay();
  nokia.display(); // show splashscreen
}

void loop() {
  nav.poll();
  nokia.display(); // show splashscreen
  //digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
