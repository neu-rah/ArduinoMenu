/********************
Arduino generic menu system
LCD menu - unsing arduino classic LCD library
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sept.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

on this example we build a menu without using the macros, its a bit harder but has more clean error messages
*/
#include <HardwareSerial.h>
#include <LiquidCrystal.h>
#include <menu.h>//menu macros and objects
#include <pcint.h>//this is incompatible with software serial (arduino needs an handler!)
#include <quadEncoder.h>//quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include "menuLCD.h"
#include "streamFlow.h"
#include <controls/common/menuTextFields.h>

#if defined(__AVR_ATmega2560__)
  //LCD wired on my AtMega2560
  #define RS 44
  #define RW 46
  #define EN 48
  LiquidCrystal lcd1(RS, RW, EN, 43, 45, 47, 49);
#elif defined(__AVR_ATmega328__)||defined (__AVR_ATmega328P__)
  #define RS 8
  #define RW 3
  #define EN 9
  LiquidCrystal lcd1(RS, RW, EN, 4, 5, 6, 7);
#else
  #error "DEFINE YOUR LCD WIRING HERE (search for this message on code)"
#endif

////////////////////////////////////////////
// ENCODER (aka rotary switch) PINS
// rotary
#if defined(__AVR_ATmega328__)||defined(__AVR_ATmega328P__)// uno and breaduinos
  #define encA 2
  #define encB 3
  //this encoder has a button here
  #define encBtn A0
  #define LEDPIN 13
#elif defined(__AVR_ATmega2560__)
  #define encA 10
  #define encB 12
  #define encBtn 13
  #define LEDPIN 5
#endif

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

//aux function
void nothing() {}

void ledOn() {digitalWrite(LEDPIN,1);}
void ledOff() {digitalWrite(LEDPIN,0);}

char name[4+1]="0000";


void updateName(prompt& p,menuOut& o,Stream &c) {
  digitalWrite(LEDPIN,!digitalRead(LEDPIN));
}

bool runMenu=true;//to disable the menu and free the display for something else, this is controlled outside the lib

void justTest() {
  runMenu=false;
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("Using the LCD");
}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
// empty options are just for scroll testing

prompt sub1("LED ON",ledOn);
prompt sub2("LED OFF",ledOff);
prompt* subMenuData[]={&sub1,&sub2};
menu subMenu("Sub-menu",2,subMenuData);

prompt empty("Empty");

prompt* mainMenuData[]={&subMenu,&empty};
menu mainMenu("Main menu",2,mainMenuData);

///////////////////////////////////////////////////////////////////////////
// IO
//the quadEncoder
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,5);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//multiple inputs allow conjugation of the quadEncoder with a single key keyboard that is the quadEncoder button
//Stream* in[]={&enc,&encButton};
//chainStream<2> quadEncoder_button(in);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton,&Serial};
chainStream<3> allIn(in3);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuLCD lcd(lcd1,16,2);

/////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("menu system test");

  quadEncoder.begin();

  lcd1.begin(16,2);
  lcd1.print("Menu test");
  //lcd1.blink();
  lcd1.noCursor();

  pinMode(encBtn, INPUT);
  digitalWrite(encBtn,1);

  pinMode(LEDPIN,OUTPUT);

  delay(300);
}

///////////////////////////////////////////////////////////////////////////////
// testing the menu system
void loop() {
    mainMenu.poll(lcd,allIn);
}
