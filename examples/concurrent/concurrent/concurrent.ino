/********************
Sept. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

Concurrent menu test
output: I2C LCD (Malpartida) and Serial https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/
input: encoder + Serial
purpose: test independent concurrent menus on two different devices
  the same menu could also be used to independent navigation on two output devices
www.r-site.net

***/

#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>//F. Malpartida LCD's driver
#include <menu.h>//menu macros and objects
#include <menuIO/lcdOut.h>//malpartidas lcd menu output
#include <menuIO/encoderIn.h>//quadrature encoder driver and fake stream
#include <menuIO/keyIn.h>//keyboard driver and fake stream (for the encoder button)
#include <menuIO/chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout

// Encoder /////////////////////////////////////
#define encA 2
#define encB 3
//this encoder has a button here
#define encBtn 4

encoderIn<encA,encB> encoder;//simple quad encoder driver
#define ENC_SENSIVITY 4
encoderInStream<encA,encB> encStream(encoder,ENC_SENSIVITY);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
menuIn* inputsList[]={&encStream,&encButton};
chainStream<2> in(inputsList);//2 is the number of inputs

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,EXIT("<Back")
);

#define MAIN_MAX_DEPTH 1

MENU_OUTPUTS(mainout,MAIN_MAX_DEPTH
  ,LCD_OUT(lcd,{0,0,16,2})
  ,NONE
);

NAVROOT(mainNav,mainMenu,MAIN_MAX_DEPTH,in,mainout);//the navigation root object

//--- define a second (aux) menu with independent IO --------------------------
MENU(auxMenu,"Aux menu",doNothing,noEvent,wrapStyle
  ,OP("Aux1",doNothing,noEvent)
  ,OP("Aux2",doNothing,noEvent)
  ,EXIT("<Back")
);

#define AUX_MAX_DEPTH 1

MENU_OUTPUTS(auxOut,AUX_MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE
);

serialIn serial(Serial);
NAVROOT(auxNav,auxMenu,AUX_MAX_DEPTH,serial,auxOut);//the navigation root object
//---------------------------

void setup() {
  pinMode(encBtn,INPUT_PULLUP);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Arduino Menu Library 4.x concurrent test");Serial.flush();
  encoder.begin();
  lcd.begin(16,2);
  mainNav.showTitle=false;
  lcd.setCursor(0, 0);
  lcd.print("Menu 4.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("r-site.net");
  delay(2000);
}

void loop() {
  mainNav.poll();
  auxNav.poll();
  delay(100);//simulate a delay as if other tasks are running
}
