#include <Arduino.h>

/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Nov. 2020 Graham Whaley - adapted to Grove RGB LCD

menu output to Grove RGB LCD I2C
output: GROVERGBLCD : https://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/
input: click encoder and Serial
board: PJRC Teensy 4.0 : https://www.pjrc.com/store/teensy40.html
***/

#include <Wire.h>
#include <menu.h>
#include <menuIO/groveRGBLCDOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

// LCD, standard Grove I2C devices /////////////
rgb_lcd lcd;

// Encoder /////////////////////////////////////
#define encA 3
#define encB 2
//this encoder has a button here
#define encBtn 4

ClickEncoder clickEncoder(encA,encB,encBtn,2);
ClickEncoderStream encStream(clickEncoder,1);

//input from the clickencoder + serial
serialIn serial(Serial);
menuIn* inputsList[]={&encStream,&serial};
chainStream<2> in(inputsList);//3 is the number of inputs

#define LEDPIN LED_BUILTIN   //Teensy 4.0 LED pin

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial.print("action1 event: ");
  Serial.print(e);
  Serial.println(", proceed menu");
  Serial.flush();
  return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item) {
  Serial.print("action2 event: ");
  Serial.print(e);
  Serial.print(", quiting menu.");
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

/*extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);*/

// char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
// char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
// char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  // ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

//const panel panels[] MEMMODE={{0,0,16,2}};
//navNode* nodes[sizeof(panels)/sizeof(panel)];
//panelsList pList(panels,nodes,1);

#define MAX_DEPTH 2
/*idx_t tops[MAX_DEPTH];
liquidCrystalOut outLCD(lcd,tops,pList);//output device for LCD
menuOut* constMEM outputs[] MEMMODE={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs*/

MENU_OUTPUTS(out, MAX_DEPTH
  ,GROVERGBLCD_OUT(lcd, {0, 0, 16, 2})
  ,NONE
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);//the navigation root object

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

result idle(menuOut& o,idleEvent e) {
  switch(e) {
    case idleStart:o.print("suspending menu!");break;
    case idling:o.print("suspended...");break;
    case idleEnd:o.print("resuming menu.");break;
  }
  return proceed;
}

void setup() {
  pinMode(encBtn,INPUT_PULLUP);
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Arduino Menu Library");Serial.flush();

  lcd.begin(16,2);
  lcd.setRGB(32, 32, 32);
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
  nav.showTitle=false;
  lcd.setCursor(0, 0);
  lcd.print("Menu 4.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("r-site.net");
  delay(2000);
}

void loop() {
  clickEncoder.service();
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay as if other tasks are running
}
