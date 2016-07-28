/********************
Arduino generic menu system
ClickEncoder example

http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

ClickEncoder: https://github.com/0xPIT/encoder

Dec.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Adaptation and mods July 2016 Christophe Persoz - christophepersoz(@rrob@)gmail.com

Thread Safe: No
Extensible: Yes
Adapted to specific projet : Yes

*/

#include <ClickEncoder.h> // Quad encoder
#include <TimerOne.h>     // ISR on ClickEncoder

#include <menu.h>
#include <menuPrint.h>
#include <ClickEncoderStream.h> // New quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuFields.h>

#define CK_ENC  2 // Quand encoder on an ISR capable input
#define DT_ENC  7
#define SW_ENC  6

ClickEncoder qEnc(DT_ENC, CK_ENC, SW_ENC, 4, LOW);

boolean runMenu = true;
signed char cIdxRunScr = 0;             // Current cursor position on run screen
signed char pIdxRunScr = -1;

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

// close menu
bool exitMenu() {
  runMenu = false;
  return true;
}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION WITH MACROS
char test=1;
int param1, param2 = 12;
int adc_prescale = 64;

TOGGLE(test,onoff,"LED: ",
  VALUE("ON",1),
  VALUE("OFF",0)
);

CHOOSE(adc_prescale,sclock,"Sample clock ",
    VALUE("/ 128",128,menu::nothing),
    VALUE("/ 64",64,menu::nothing),
    VALUE("/ 32",32,menu::nothing),
    VALUE("/ 16",16,menu::nothing),
    VALUE("/ 8",8,menu::nothing)
);

MENU(mainMenu,"Main menu 1",
  SUBMENU(onoff),
  OP("DO NOTHING",menu::nothing),
  OP("DO SOMETHING",menu::nothing),
  SUBMENU(sclock),
  FIELD(param1,"VALUE","%",0,100,10,1),
  OP("< BACK",exitMenu)
);

/* Quad encoder */
ClickEncoderStream enc(qEnc, 1);// simple quad encoder fake Stream

//alternative to previous but now we can input from Serial too...
Stream* menuInputs[]={&enc,&Serial};
chainStream<2> allIn(menuInputs);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuPrint menu_out(Serial);//describe output device

void timerIsr() {
  qEnc.service();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  Serial.println("Menu system test");

  // Encoder init
  qEnc.setAccelerationEnabled(false);
  qEnc.setDoubleClickEnabled(true); // must be on otherwise the menu library Hang
  //displayAccelerationStatus();

  // ISR init
  Timer1.initialize(5000); // every 0.05 seconds
  Timer1.attachInterrupt(timerIsr);

  pinMode(13,OUTPUT);
}

void loop() {
    digitalWrite(13,test);
    if (!runMenu)  {
      if (allIn.read() == menu::enterCode) {
        runMenu = true;
        mainMenu.sel = 0; // reset the menu index fornext call
      } else {
        Serial.println("www.r-site.net");
        delay(100);
      }
    } else
      mainMenu.poll(menu_out,allIn); // temporary control and display on Serial Monitor
}
