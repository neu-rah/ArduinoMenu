/***************************************
Arduino generic menu system
GFX menu - unsing adafruit gfx screens
tested on ATmega2560
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no 
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

Adafruit GFX library from:
  https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
Display ST7735 driver
  http://www.adafruit.com/product/358
***/
/******************
Rui Azevedo 2014
ruihfazevedo@rrob@gmail.com
********/
#include <SPI.h>
#include <menu.h>//menu macros and objects
#include <pcint.h>//this is incompatible with software serial (arduino needs an handler!)
#include <quadEncoder.h>//quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuPrint.h>//Print (Serial) menu
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <menuGFX.h>

#if defined(__AVR_ATmega2560__)
  ///////////////////////////////////////////////////////////////////////////
  //TFT + SD
  //#define sdCS  9//not using SD card
  #define tftCS 8
  #define dc    6
  #define rst   7
  ////////////////////////////////////////////
  // ENCODER (aka rotary switch) PINS
  #define encA 10
  #define encB 12
  #define encBtn 13
#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
  ///////////////////////////////////////////////////////////////////////////
  //TFT + SD
  //#define sdCS  9//not using SD card
  #define tftCS 10
  #define dc    7
  #define rst   8
  ////////////////////////////////////////////
  // ENCODER (aka rotary switch) PINS
  #define encA A2
  #define encB A1
  #define encBtn A3
#else
  #error "Uknown pinout"
#endif

Adafruit_ST7735 tft(tftCS, dc, rst);
///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

//aux function
void nothing() {}

void setValue(int &value,prompt &p,menuOut &o, Stream &i,const char* text,const char* units="",int sensivity=5,int low=0,int hi=100,int steps=0,void (*func)()=nothing);

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
MENU(subMenu,"Sub-Menu",
  OP("Op1",nothing),
  OP("Op2",nothing),
  OP("Op3",nothing)
);

MENU(mainMenu,"Sistema",
  OP("A",nothing),
  OP("B",nothing),
  SUBMENU(subMenu)
);

//the quadEncoder
quadEncoder encoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(encoder,5);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//multiple inputs allow conjugation of the quadEncoder with a single key keyboard that is the quadEncoder button
Stream* in[]={&enc,&encButton};
chainStream<2> quadEncoder_button(in);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton};
chainStream<3> allIn(in3);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuGFX gfx(tft);

/////////////////////////////////////////////////////////////////////////
void setup() {
  SPI.begin();
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.setTextWrap(false);
  tft.setTextColor(ST7735_RED,ST7735_BLACK);
  tft.setTextSize(2);
  gfx.resX*=2;//update resolution after font size change
  gfx.resY*=2;//update resolution after font size change
  tft.fillScreen(ST7735_BLACK);
  tft.print("Menu test on GFX");
  //testing menu limits (not using all the screen)
  //size is within screen limits even after rotation
  //this limits are not constrained, please ensure your text fits
  gfx.maxX=8;
  gfx.maxY=3;
  gfx.bgColor=SILVER;

  pinMode(encBtn, INPUT);
  digitalWrite(encBtn,1);
  
  encoder.begin();

  delay(300);
  tft.fillScreen(GREEN);
}

///////////////////////////////////////////////////////////////////////////////
// testing the menu system
void loop() {
  mainMenu.poll(gfx,allIn);
}

