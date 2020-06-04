/*********************
 * Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
 * May 2020 - review by: Neftali Loya Garibay

 * This menu example works for Chinese MCUFRIEND_kbv TFT LCD's normally sold by Banggood and
 * alike places. It is based on adafruit GFX library and on its own "MCUFRIEND_kbv.h" library,
 * downlodble from gitHub. It was tested on an ATMEGA-2560 board and it uses an encoder as the
 * navigation device. The encoder has also a switch and it was used as the enter key.
 *
 * Output: 3.5 inch, 480*320 TFT-LCD Arduino UNO and MEGA shield, ILI9486.
 * Input: Serial + encoder
 *
 * www.r-site.net
 *
 * Alternative encoder (clickEncoder) uses:
 * https://github.com/0xPIT/encoder
 * https://github.com/PaulStoffregen/TimerOne
 *
*********************/

#include <Arduino.h>

//**** TFT-LCD RELATED LIBRARIES ***
#include <Wire.h>
#include "Adafruit_GFX.h"
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

//*** MENU LIBRARY AND ASSOCIATED ENCODER LIBRARIES ***
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#include <TimerOne.h>
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN 42

//*** ROTARY ENCODER PINS AS USED ON AN ARDUINO MEGA-2560 ***
#define encBtn  44  //  Use as input with pull-ups
#define encB    46  //  Use as input with pull-ups
#define encC    48  //  Set it to GND
#define encA    50  //  Use as input with pull-ups
#define encBtn2 52  //  Set it as output to GND

/*  HUMAN READABLE COLOR DEFINITIONS. Not all the following color defiitions are used on the
 *  example. However, it is handy to have them available for tailoring the menu's look according
 *  to our liking. These definitions are a Bodmer's contribution.
 */
#define BLACK       0x0000      /*    0,   0,   0 */
#define NAVY        0x000F      /*    0,   0, 128 */
#define DARKGREEN   0x03E0      /*    0, 128,   0 */
#define DARKCYAN    0x03EF      /*    0, 128, 128 */
#define MAROON      0x7800      /*  128,   0,   0 */
#define PURPLE      0x780F      /*  128,   0, 128 */
#define OLIVE       0x7BE0      /*  128, 128,   0 */
/*#define LIGHTGREY   0xC618    /*  128, 128,   0 */
#define GRAY        0xC618      /*  192, 192, 192 */
#define DARKGRAY    0x7BEF      /*  128, 128, 128 */
#define BLUE        0x001F      /*    0,   0, 255 */
#define GREEN       0x07E0      /*    0, 255,   0 */
#define CYAN        0x07FF      /*    0, 255, 255 */
#define RED         0xF800      /*  255,   0,   0 */
#define MAGENTA     0xF81F      /*  255,   0, 255 */
#define YELLOW      0xFFE0      /*  255, 255,   0 */
#define WHITE       0xFFFF      /*  255, 255, 255 */
#define ORANGE      0xFDA0      /*  255, 180,   0 */
#define GREENYELLOW 0xB7E0      /*  180, 255,   0 */
#define PINK        0xFC9F


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

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
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

//*** DEFINE MENU COLORS ***______________________________________________________________________________
const colorDef<uint16_t> colors[6] MEMMODE={
  {{(uint16_t)BLACK,(uint16_t)BLACK}, {(uint16_t)BLACK, (uint16_t)BLUE,  (uint16_t)BLUE}},  //bgColor
  {{(uint16_t)GRAY, (uint16_t)GRAY},  {(uint16_t)WHITE, (uint16_t)WHITE, (uint16_t)WHITE}}, //fgColor
  {{(uint16_t)WHITE,(uint16_t)BLACK}, {(uint16_t)YELLOW,(uint16_t)YELLOW,(uint16_t)RED}},   //valColor
  {{(uint16_t)WHITE,(uint16_t)BLACK}, {(uint16_t)WHITE, (uint16_t)YELLOW,(uint16_t)YELLOW}},//unitColor
  {{(uint16_t)WHITE,(uint16_t)GRAY},  {(uint16_t)BLACK, (uint16_t)BLUE,  (uint16_t)WHITE}}, //cursorColor
  {{(uint16_t)WHITE,(uint16_t)YELLOW},{(uint16_t)BLUE,  (uint16_t)RED,   (uint16_t)RED}},   //titleColor
};

serialIn serial(Serial);

#ifdef USE_CLICK_ENCODER
  ClickEncoder clickEncoder(encA,encB,encBtn);
  ClickEncoderStream encStream(clickEncoder,1);
  MENU_INPUTS(in,&encStream,&serial);
  void timerIsr() {clickEncoder.service();}
#else
  encoderIn<encA,encB> encoder;//simple quad encoder driver
  encoderInStream<encA,encB> encStream(encoder,1);// simple quad encoder fake Stream
  //a keyboard with only one key as the encoder button
  keyMap encBtn_map[]={{-encBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
  keyIn<1> encButton(encBtn_map);//1 is the number of keys
  MENU_INPUTS(in,&encStream,&encButton,&serial);
#endif

#define MAX_DEPTH 5
#define textScale 2
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(tft,colors,6*textScale,9*textScale,{0,0,14,8},{14,0,14,8})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//*** WHEN MENU IS SUSPENDED ***
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.println(F("suspended..."));
    o.println(F("press [select]"));
    o.println(F("to continue"));
  }
  return proceed;
}

//  config myOptions('*','-',defaultNavCodes,false);

void setup() {
    uint16_t ID;
    Serial.begin(9600);
    Serial.print("Show BMP files on TFT with ID:0x");
    ID = tft.readID();
    Serial.println(ID, HEX);
    if (ID == 0x0D3D3) ID = 0x9481;
    tft.begin(ID);
    tft.fillScreen(0x001F);
    tft.setTextColor(0xFFFF, 0x0000);

  //options=&myOptions;//can customize option
  pinMode(encBtn, INPUT_PULLUP);
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  pinMode(encC, OUTPUT-LOW);
  pinMode(encBtn2, OUTPUT-LOW);
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x test");
  Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].disable();
  //outGfx.usePreview=true;//reserve one panel for preview?
  //nav.showTitle=false;//show menu title?


  #ifdef USE_CLICK_ENCODER
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
  #else
    encButton.begin();
    encoder.begin();
  #endif

  tft.setRotation(3);
  tft.setTextSize(textScale);     //  test scalling
  //tft.setTextWrap(false);       //  NOT AVAILABLE WITHIN THE "MCUFRIEND_kbv.h" library
  tft.fillScreen(BLACK);
  tft.setTextColor(RED,BLACK);
  tft.println("Menu 4.x T est on GFX");
  delay(1000);
}

void loop() {
  nav.poll();                     //  This device only draws when needed.
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);                     //  Simulate a delay when other tasks are done.
}
