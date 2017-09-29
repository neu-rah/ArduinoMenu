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
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/serialOut.h>

using namespace Menu;

//PCD8544 aka nokia5110
#define GFX_DC  9
#define GFX_CS  8
#define GFX_RST 7

Adafruit_PCD8544 gfx(GFX_DC,GFX_CS,GFX_RST);

#define LEDPIN A3

// rotary encoder pins
/*#define encA    5
#define encB    6
#define encBtn  4*/

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print(F("event:"));
  Serial.print(e);
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

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<"doAlert nav:";
  Serial.println((long)&nav);
  Serial<<"nav.root:";
  Serial.println((long)nav.root);
  //nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
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

#define gfxWidth 84
#define gfxHeight 48
#define fontX 5
#define fontY 9
#define MAX_DEPTH 2

#define MAX_DEPTH 2
#define textScale 1
// MENU_OUTPUTS(out,MAX_DEPTH
//   ,ADAGFX_OUT(gfx,colors,fontX,fontY,{0,0,gfxWidth/fontX,gfxHeight/fontY})
//   ,SERIAL_OUT(Serial)
// );

//NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//initializing output and menu nav without macros
const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
panelsList default_serial_panel_list(
  default_serial_panels,
  default_serial_nodes,
  sizeof(default_serial_panels)/sizeof(panel)
);

//define output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(*(Print*)&Serial,serialTops);

//define outputs controller
idx_t gfx_tops[MAX_DEPTH];
PANELS(gfxPanels,{0,0,gfxWidth/fontX,gfxHeight/fontY});
adaGfxOut adaOut(gfx,colors,gfx_tops,gfxPanels);

menuOut* const outputs[] MEMMODE={&outSerial,&adaOut};//list of output devices
outputsList out(outputs,2);//outputs list controller

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
// navRoot nav(mainMenu, nav_cursors, MAX_DEPTH-1, Serial, out);
navRoot *nav;

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  //if (e==idling)
    o.println(F("suspended..."));
    o.println(F("press [select]"));
    o.println(F("to continue"));
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("menu 4.x test"));
  nav=new navRoot(mainMenu, nav_cursors, MAX_DEPTH-1, Serial, out);
  Serial<<"root:"<<(long)nav<<endl;
  Serial<<"path:"<<(long)nav->path<<endl;
  for(int n=0;n<nav->maxDepth;n++) {
    Serial<<n<<":"<<(long)&nav->path[n]<<"->"<<(long)nav->path[n].root<<endl;
  }
  Serial.flush();
  nav->idleTask=idle;//point a function to be used when menu is suspended
  //mainMenu[1].enabled=disabledStatus;

  //pinMode(encBtn, INPUT_PULLUP);
  //encoder.begin();

  SPI.begin();
  gfx.begin();
  gfx.clearDisplay();
  gfx.println(F("Menu 4.x test on GFX"));
  gfx.setContrast(50);
  gfx.display(); // show splashscreen
  delay(2000);
  gfx.clearDisplay();
  gfx.display(); // show splashscreen
}

void loop() {
  //nav.poll();//it can work like this, followed by the gfx.display()
  //gfx.display();

  //or on a need to draw basis:
  nav->doInput();
  if (nav->changed(0)) {//only draw if changed
    nav->doOutput();
    gfx.display();
  }

  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
