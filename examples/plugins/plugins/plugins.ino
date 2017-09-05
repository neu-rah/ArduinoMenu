/* -*- C++ -*- */
/********************
May 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

www.r-site.net

unlike native objects, pluggins can be device specific, so some pluggins might
not work on all devices.

all sorts of plugins may be available in the future and if you customize
a component and think it of interest of others please do pull request.

contrubutions and contrubutors are welcome.

Cancelable fields - restore original value on escape (long encoder press)
barField - numeric field edit with a graph bar

changes:
ITEM macro, like OP but do allow custom system styles and extra arguments at end

***/

#include <Arduino.h>

#include <menu.h>
#ifdef DEBUG
#include <Dump.h>
#endif

//inputs
#include <menuIO/encoderIn.h>
#include <menuIO/softKeyIn.h>
#include <menuIO/chainStream.h>

//outputs
#include <menuIO/serialOut.h>
#include <menuIO/liquidCrystalOut.h>

//include plugins
#include <plugin/cancelField.h>
#include <plugin/barField.h>

using namespace Menu;

int test=55;

char* name="Edit me...";

#define LEDPIN 13

// LCD /////////////////////////////////////////
#define RS 8
#define RW 3
#define EN 9
LiquidCrystal lcd(RS, RW, EN, 4, 5, 6, 7);

// Encoder /////////////////////////////////////
#define encA A2
#define encB 2
//this encoder has a button here
#define encBtn A3

encoderIn<encA,encB> encoder;//simple quad encoder driver
encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
softKeyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
Stream* inputsList[]={&encStream,&encButton,&Serial};
chainStream<3> in(inputsList);//3 is the number of inputs

char* const hexDigit PROGMEM="0123456789ABCDEF";
char* const hexNr[] PROGMEM={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

//a menu using a plugin field
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,BARFIELD(test,"Bar field","%",0,100,10,1,doNothing,noEvent,wrapStyle)//numeric field with a bar
  ,FIELD(test,"Original","%",0,100,10,1,doNothing,noEvent,wrapStyle)//normal numeric field (2 edit levels)
  ,FIELD(test,"O. Simple","%",0,100,1,0,doNothing,noEvent,wrapStyle)//normal numeric field (1 edit level)
  ,altFIELD(cancelField,test,"Cancelable","%",0,100,10,1,doNothing,enterEvent,wrapStyle)//cancelable field (2 edit levels)
  ,altFIELD(cancelField,test,"C. Simple","%",0,100,1,0,doNothing,enterEvent,wrapStyle)//cancelable field (1 edit level)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Exit")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,LIQUIDCRYSTAL_OUT(lcd,{0,0,16,2})
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);//the navigation root object

void setup() {
  pinMode(encBtn,INPUT_PULLUP);
  //pinMode(LEDPIN,OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  encoder.begin();
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("Menu 3.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("r-site.net");
  options->numValueInput=false;//numeric keys in fields used as accelerators instead
  //Serial<<"menu 3.x plugins"<<endl;Serial.flush();
  //setting some plugins otions
  barFieldOptions::fill="\xFF";
  //barFieldOptions::empty="░";//if not stick with the defaults
  cancelFieldOptions::quitOnEsc=true;
  cancelFieldOptions::accelSendEsc=true;//normal is enter
  #ifdef DEBUG
  dumpPgm(Serial,(textFieldShadowRaw*)mainMenu[5].shadow,sizeof(textFieldShadowRaw));
  #endif
}

void loop() {
  nav.poll();
  delay(100);//simulate other tasks delay
}
