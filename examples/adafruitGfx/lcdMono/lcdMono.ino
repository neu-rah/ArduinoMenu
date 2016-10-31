#include <Arduino.h>

/********************
Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu with adafruit GFX
output: Nokia 5110 display (PCD8544 HW SPI)
input: Serial
www.r-site.net

note: adafruit's gfx buffer eats too much ram on this device
=> removed encoder to save ram

***/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <menu.h>
#include <dev/adafruitGfxOut.h>

using namespace Menu;

//PCD8544 aka nokia5110
#define GFX_DC  9
#define GFX_CS  8
#define GFX_RST 7

Adafruit_PCD8544 gfx(GFX_DC,GFX_CS,GFX_RST);

#define LEDPIN A3

// rotary encoder pins
/*#define encA    2
#define encB    3
#define encBtn  4*/

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;
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
  idx_t printTo(navRoot &root,bool sel,menuOut& out,idx_t len) override {
    return out.printRaw("special prompt!",len);;
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling)
    o<<"alert test"<<endl<<"press [select]"<<endl<<"to continue..."<<endl;
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
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
  {{WHITE,BLACK},{WHITE,BLACK,BLACK}},//bgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//fgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//valColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//unitColor
  {{BLACK,WHITE},{WHITE,WHITE,WHITE}},//cursorColor
  {{BLACK,WHITE},{WHITE,BLACK,BLACK}},//titleColor
};

/*encoderIn encoder(encA,encB);//simple quad encoder driver
encoderInStream encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
Stream* inputsList[]={&encStream,&encButton,&Serial};
chainStream<3> in(inputsList);//3 is the number of inputs*/

/*const panel serial_panels[] MEMMODE={{0,0,40,10}};
menuNode* serial_nodes[sizeof(serial_panels)/sizeof(panel)];
panelsList serial_pList(serial_panels,serial_nodes,sizeof(serial_panels)/sizeof(panel));
serialOut outSerial(Serial,serial_pList);//the output device (just the serial port)*/

#define fontX 5
#define fontY 9
const panel panels[] MEMMODE={{0,0,84/fontX,48/fontY}};
menuNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,sizeof(panels)/sizeof(panel));

menuGFX outGFX(gfx,colors,pList,fontX,fontY);//output device for LCD with 5x8 font size
menuOut* outputs[]={&outGFX};
outputsList out(outputs,1);
#define MAX_DEPTH 2
NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling)
    o<<F("suspended...")<<endl<<F("press [select]")<<endl<<F("to continue");
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial<<F("menu 3.0 test")<<endl;Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  //mainMenu[1].enabled=disabledStatus;

  //pinMode(encBtn, INPUT_PULLUP);
  //encoder.begin();

  SPI.begin();
  gfx.begin();
  gfx.clearDisplay();
  gfx<<F("Menu 3.x test on GFX")<<endl;;
  gfx.setContrast(50);
  gfx.display(); // show splashscreen
  delay(2000);
  gfx.clearDisplay();
  gfx.display(); // show splashscreen
}

void loop() {
  //nav.poll();//it can work like this, followed by the gfx.display()
  //or on a need to draw basis
  nav.doInput();
  if (nav.changed(outGFX)) {
    nav.doOutput();
    gfx.display(); // show splashscreen
  }
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
