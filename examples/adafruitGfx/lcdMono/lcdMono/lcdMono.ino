#include <Arduino.h>

/********************
Sept. 2014 ~ Oct 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu with adafruit GFX
output: Nokia 5110 display (PCD8544 HW SPI) + Serial
input: Encoder + Serial
www.r-site.net

***/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <menu.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

//PCD8544 aka nokia5110
#define GFX_DC  9
#define GFX_CS  8
#define GFX_RST 7

Adafruit_PCD8544 gfx(GFX_DC,GFX_CS,GFX_RST);

#define LEDPIN A3

// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial.print(F("event:"));
  Serial.print(e);
  return proceed;
}

int test=55;
int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

result alert(menuOut& o,idleEvent e);
result doAlert(eventMask e, prompt &item);

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table
const colorDef<uint16_t> colors[6] MEMMODE={
  {{WHITE,BLACK},{WHITE,BLACK,BLACK}},//bgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//fgColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//valColor
  {{BLACK,WHITE},{BLACK,WHITE,WHITE}},//unitColor
  {{BLACK,WHITE},{WHITE,WHITE,WHITE}},//cursorColor
  {{BLACK,WHITE},{WHITE,BLACK,BLACK}},//titleColor
};

#define gfxWidth 84
#define gfxHeight 48
#define fontX 6
//5
#define fontY 9
#define MAX_DEPTH 2

encoderIn<encA,encB> encoder;//simple quad encoder driver
encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

serialIn serial(Serial);
MENU_INPUTS(in,&encStream,&encButton,&serial);

#define MAX_DEPTH 2
#define textScale 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(gfx,colors,fontX,fontY,{0,0,gfxWidth/fontX,gfxHeight/fontY})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//initializing output and menu nav without macros
/*const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
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

//define input device
serialIn serial(Serial);

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, serial, out);*/

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print(F("alert test"));
    o.setCursor(0,1);
    o.print(F("press [select]"));
    o.setCursor(0,2);
    o.print(F("to continue..."));
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  o.setCursor(0,0);
  o.print(F("suspended..."));
  o.setCursor(0,1);
  o.print(F("press [select]"));
  o.setCursor(0,2);
  o.print(F("to continue"));
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("menu 4.x test"));
  Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended

  encButton.begin();
  encoder.begin();

  SPI.begin();
  gfx.begin();
  gfx.clearDisplay();
  gfx.println(F("Menu 4.x test on GFX"));
  gfx.setContrast(50);
  gfx.display(); // show splashscreen
  delay(2000);
  gfx.clearDisplay();
  gfx.display(); // show splashscreen
  // gfx.drawRect(0, 0, 84, 48, 1);
}

void loop() {
  //nav.poll();//it can work like this, followed by the gfx.display()
  //gfx.display();

  //or on a need to draw basis:
  nav.doInput();
  if (nav.changed(0)) {//only draw if changed
    nav.doOutput();
    gfx.display();
  }

  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
