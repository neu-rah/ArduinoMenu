#include <Arduino.h>

/********************
Arduino generic menu system
lolin32 menu example

output: onboard oled (i2c ssd1306 u8g2)
input: Serial
mcu: esp32 lolin with builtin oled

*/

#include <menu.h>
#include <menuIO/u8g2Out.h>
// #include <menuIO/encoderIn.h>
// #include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

// #define LEDPIN LED_BUILTIN
#define SDA 5
#define SCL 4

#include <Wire.h>
#define fontName u8g2_font_7x13_mf
#define fontX 7
#define fontY 16
#define offsetX 0
#define offsetY 3
#define U8_Width 128
#define U8_Height 64
#define USE_HWI2C
#define fontMarginX 2
#define fontMarginY 2
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);
U8G2_SSD1306_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);//allow contrast change
// U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);


// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[6] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,1},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{1,1},{1,0,0}},//titleColor
};

result doAlert(eventMask e, prompt &item);

int test=55;

int ledCtrl=HIGH;

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
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t panelNr) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,OP("Sub1",doNothing,noEvent)
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,EXIT("<Back")
);

uint16_t hrs=0;
uint16_t mins=0;

//define a pad style menu (single line menu)
//here with a set of fields to enter a date in YYYY/MM/DD format
altMENU(menu,timeMenu,"Time",doNothing,noEvent,noStyle,(systemStyles)(_asPad|Menu::_menuData|Menu::_canNav|_parentDraw)
  ,FIELD(hrs,"",":",0,11,1,0,doNothing,noEvent,noStyle)
  ,FIELD(mins,"","",0,59,10,1,doNothing,noEvent,wrapStyle)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={hexDigit};
char buf1[]="0000";

const char* constMEM alphaNum MEMMODE=" 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
const char* constMEM alphaNumMask[] MEMMODE={alphaNum};
char name[]="          ";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,EDIT("Name",name,alphaNumMask,doNothing,noEvent,noStyle)
  ,SUBMENU(timeMenu)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Exit")
);

#define MAX_DEPTH 2

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

//define output device serial
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(*(Print*)&Serial,serialTops);

// MENU_OUTPUTS(out,MAX_DEPTH
//   ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,U8_Width/fontX,U8_Height/fontY})
//   ,SERIAL_OUT(Serial)
// );//can use this macro or the following lines to define customized outputs

//define output device oled
idx_t gfx_tops[MAX_DEPTH];
PANELS(gfxPanels,{0,0,U8_Width/fontX,U8_Height/fontY});
u8g2Out oledOut(u8g2,colors,gfx_tops,gfxPanels,fontX,fontY,offsetX,offsetY,fontMarginX,fontMarginY);

//define outputs controller
menuOut* outputs[]{&outSerial,&oledOut};//list of output devices
outputsList out(outputs,sizeof(outputs)/sizeof(menuOut*));//outputs list controller

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

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

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");break;
  }
  return proceed;
}

void setup() {
  // pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");Serial.flush();
  Wire.begin(SDA,SCL);
  u8g2.begin();
  u8g2.setFont(fontName);
  // disable second option
  mainMenu[1].enabled=disabledStatus;
  nav.idleTask=idle;//point a function to be used when menu is suspended
  Serial.println("setup done.");Serial.flush();
  u8g2.firstPage();
  do {
    u8g2.drawStr(0,fontY,"ArduinoMenu 4.x");
    u8g2.drawStr(0,fontY<<1,"on lolin32");
    u8g2.drawStr(0,fontY+(fontY<<1),"with buitin oled");
  } while(u8g2.nextPage());
  for(int c=255;c>0;c--) {
    u8g2.setContrast(255-255.0*log(c)/log(255));
    delay(8);
  }
  u8g2.setContrast(255);
  delay(500);
  nav.timeOut=60;//seconds
}

void loop() {
  nav.doInput();
  // digitalWrite(LEDPIN, !ledCtrl);//no led on this board
  if (nav.changed(0)) {//only draw if menu changed for gfx device
    u8g2.firstPage();
    do nav.doOutput(); while(u8g2.nextPage());
  }
  delay(100);//simulate other tasks delay
}
