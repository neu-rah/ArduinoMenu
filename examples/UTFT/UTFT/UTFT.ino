/********************
Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu with UTFT (tested on arduino due)
output: 3.2″ TFT LCD Module Display 240X320
input: Serial + Touch Panel
www.r-site.net

UTFT library from:
  http://www.rinkydinkelectronics.com/library.php?id=51
  http://www.rinkydinkelectronics.com/library.php?id=92

Note: I was unable to build for esp8266 - neu-rah
***/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/utftOut.h>
#include <menuIO/urtouchIn.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

UTFT tft(CTE28,25,26,27,28);
//extern uint8_t SmallFont[];
extern uint8_t BigFont[];
//extern uint8_t SevenSegNumFont[];

#define LEDPIN 13

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test=55;

result action1(eventMask e) {
  Serial.print(e);
  Serial.println(" action1 executed, proceed menu");Serial.flush();
  return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item) {
  Serial.print(e);
  Serial.println(" action2 executed, quiting menu");
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
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,noStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,noStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table

const colorDef<uint16_t> colors[6] MEMMODE={
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

idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(Serial,serialTops);
MENU_OUTLIST(out,&outGfx,&outSerial);

extern navRoot nav;
URTouch  uTouch( 6, 5, 4, 3, 2);
menuUTouch touchPanel(uTouch,nav,outGfx);
serialIn serial(Serial);
MENU_INPUTS(in,&touchPanel,&serial);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setColor(fgColor);
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

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setColor(fgColor);
    o.println("suspended...");
    o.println("press [select]");
    o.println("to continue");
    o.println(millis()%1000);
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  while(!Serial);
  Serial.begin(115200);
  Serial.println("menu 4.x UTFT + URTouch");Serial.flush();
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
  outGfx.println("Menu 4.x on UTFT");
  delay(1000);
  tft.clrScr();
}

void loop() {
  nav.poll();//this device only draws when needed
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
