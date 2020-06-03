#include <Arduino.h>

/********************
Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu with adafruit GFX
output: 1.8" TFT 128*160 (ST7735 HW SPI)
input: Serial + encoder
www.r-site.net

alternative encoder (clickEncoder) uses:
https://github.com/0xPIT/encoder
https://github.com/PaulStoffregen/TimerOne
***/

//removed as extra libs are not included into travis
// #define USE_CLICK_ENCODER

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#ifdef USE_CLICK_ENCODER
  #include <TimerOne.h>
  #include <ClickEncoder.h>
  #include <menuIO/clickEncoderIn.h>
#else
  #include <menuIO/encoderIn.h>
#endif
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

//ST7735 1.8TFT 128x160
#define TFT_CS   A1
#define TFT_DC   A0
#define TFT_RST  A2

Adafruit_ST7735 gfx(TFT_CS, TFT_DC, TFT_RST);

#define LEDPIN A3

// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

result doAlert(eventMask e, prompt &item);

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

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT("<Back")
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table

#define ST7735_GRAY RGB565(128,128,128)

const colorDef<uint16_t> colors[6] MEMMODE={
  {{(uint16_t)ST7735_BLACK,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_BLACK, (uint16_t)ST7735_BLUE,  (uint16_t)ST7735_BLUE}},//bgColor
  {{(uint16_t)ST7735_GRAY, (uint16_t)ST7735_GRAY},  {(uint16_t)ST7735_WHITE, (uint16_t)ST7735_WHITE, (uint16_t)ST7735_WHITE}},//fgColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_YELLOW,(uint16_t)ST7735_YELLOW,(uint16_t)ST7735_RED}},//valColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_WHITE, (uint16_t)ST7735_YELLOW,(uint16_t)ST7735_YELLOW}},//unitColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_GRAY},  {(uint16_t)ST7735_BLACK, (uint16_t)ST7735_BLUE,  (uint16_t)ST7735_WHITE}},//cursorColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_YELLOW},{(uint16_t)ST7735_BLUE,  (uint16_t)ST7735_RED,   (uint16_t)ST7735_RED}},//titleColor
};

serialIn serial(Serial);

#ifdef USE_CLICK_ENCODER
  ClickEncoder clickEncoder(encA,encB,encBtn);
  ClickEncoderStream encStream(clickEncoder,1);
  MENU_INPUTS(in,&encStream,&serial);
  void timerIsr() {clickEncoder.service();}
#else
  encoderIn<encA,encB> encoder;//simple quad encoder driver
  encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream
  //a keyboard with only one key as the encoder button
  keyMap encBtn_map[]={{-encBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
  keyIn<1> encButton(encBtn_map);//1 is the number of keys
  MENU_INPUTS(in,&encStream,&encButton,&serial);
#endif

#define MAX_DEPTH 4
#define textScale 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(gfx,colors,6*textScale,9*textScale,{0,0,14,8},{14,0,14,8})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.println(F("suspended..."));
    o.println(F("press [select]"));
    o.println(F("to continue"));
  }
  return proceed;
}

//config myOptions('*','-',defaultNavCodes,false);

void setup() {
  //options=&myOptions;//can customize options
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");
  Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].disable();
  //outGfx.usePreview=true;//reserve one panel for preview?
  //nav.showTitle=false;//show menu title?

  //pinMode(encBtn, INPUT_PULLUP);
  #ifdef USE_CLICK_ENCODER
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
  #else
    encButton.begin();
    encoder.begin();
  #endif

  SPI.begin();
  gfx.initR(INITR_BLACKTAB);
  gfx.setRotation(3);
  gfx.setTextSize(textScale);//test scalling
  gfx.setTextWrap(false);
  gfx.fillScreen(ST7735_BLACK);
  gfx.setTextColor(ST7735_RED,ST7735_BLACK);
  gfx.println("Menu 4.x test on GFX");
  delay(1000);
}

void loop() {
  nav.poll();//this device only draws when needed
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done
}
