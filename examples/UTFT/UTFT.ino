/********************
Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 4.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu with UTFT (tested on arduino due)
output: 3.2″ TFT LCD Module Display 240X320
input: Serial + Touch Panel
www.r-site.net

UTFT library from:
  http://www.rinkydinkelectronics.com/library.php?id=51
  http://henningkarlsen.com/electronics/library.php?id=50 (old address)


***/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/utftOut.h>
#include <menuIO/utouchIn.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>

using namespace Menu;

UTFT tft(CTE28,25,26,27,28);
//extern uint8_t SmallFont[];
extern uint8_t BigFont[];
//extern uint8_t SevenSegNumFont[];

#define LEDPIN 13

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
    return out.printRaw("special prompt!",len);;
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setColor(fgColor);
    o<<"alert test"<<endl<<"press [select]"<<endl<<"to continue..."<<endl;
  }
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,noStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,noStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table

const colorDef<uint16_t> colors[] MEMMODE={
  {{VGA_BLACK,VGA_BLACK},{VGA_BLACK,VGA_BLUE,VGA_BLUE}},//bgColor
  {{VGA_GRAY,VGA_GRAY},{VGA_WHITE,VGA_WHITE,VGA_WHITE}},//fgColor
  {{VGA_WHITE,VGA_BLACK},{VGA_YELLOW,VGA_YELLOW,VGA_RED}},//valColor
  {{VGA_WHITE,VGA_BLACK},{VGA_WHITE,VGA_YELLOW,VGA_YELLOW}},//unitColor
  {{VGA_WHITE,VGA_GRAY},{VGA_BLACK,VGA_BLUE,VGA_WHITE}},//cursorColor
  {{VGA_WHITE,VGA_YELLOW},{VGA_BLUE,VGA_RED,VGA_RED}},//titleColor
};

//PANELS(serial_panels,{0,0,40,10});//or use default
//serialOut outSerial(Serial);//,serial_panels);//the output device (just the serial port)

#define MAX_DEPTH 2

PANELS(gfx_panels,{0,0,12,8},{13,0,12,8});
idx_t gfx_tops[MAX_DEPTH];
utftOut outGfx(tft,colors,gfx_tops,gfx_panels,16,16);//output device, latter set resolution from font measure

MENU_OUTLIST(out,&outGfx);

extern navRoot nav;
UTouch  uTouch( 6, 5, 4, 3, 2);
menuUTouch touchPanel(uTouch,nav,outGfx);

NAVROOT(nav,mainMenu,MAX_DEPTH,touchPanel,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setColor(fgColor);
    o<<"suspended..."<<endl<<"press [select]"<<endl<<"to continue"<<endl<<(millis()%1000);
  }
  return proceed;
}

config myOptions('>','-',false,false,defaultNavCodes);

void setup() {
  options=&myOptions;
  pinMode(LEDPIN,OUTPUT);
  while(!Serial);
  Serial.begin(115200);
  Serial<<"menu 3.x"<<endl;Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  //mainMenu[1].enabled=disabledStatus;

  tft.InitLCD();
  tft.setBrightness(4);
  tft.clrScr();

  uTouch.InitTouch();
  uTouch.setPrecision(PREC_MEDIUM);//LOW, MEDIUM, HI, EXTREME

  tft.setFont(BigFont);
  tft.setColor(0, 255, 0);
  tft.setBackColor(0, 0, 0);

  //outGfx.resX=tft.getFontXsize()+1;
  //outGfx.resY=tft.getFontYsize()+1;
  outGfx<<"Menu 3.x on UTFT"<<endl;
  delay(1000);
  tft.clrScr();
}

void loop() {
  nav.poll();//this device only draws when needed
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
