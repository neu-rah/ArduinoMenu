/********************
Arduino generic menu system
UTFT and UTouch example for arduino due
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Dec.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

UTFT library from:
  http://henningkarlsen.com/electronics/library.php?id=50
UTouch library from:
  http://henningkarlsen.com/electronics/library.php?id=56
*/
//#define DEBUG
#include <UTFT.h>
#include <UTouch.h>
#ifdef DEBUG
#include <streamFlow.h>
#endif
#include <menu.h>
#include <menuUTFT.h>
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuUTouch.h>
#include <menuFields.h>

UTFT myGLCD(CTE28,25,26,27,28);
//extern uint8_t SmallFont[];
extern uint8_t BigFont[];
//extern uint8_t SevenSegNumFont[];

menuUTFT gfx(myGLCD);

UTouch  myTouch( 6, 5, 4, 3, 2);
menuUTouch menuTouch(myTouch,gfx);

Stream* in2[]={&menuTouch,&Serial};
chainStream<2> in(in2);

/////////////////////////////////////////////////////////////////////////
// MENU FUNCTION
// this functions will be wired to menu options
// meaning they will be called on option click/select
// or on field value change/update
bool sayIt(prompt& p,menuOut& o,Stream &c) {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.clrScr();
  myGLCD.setColor(0, 255, 0);
  myGLCD.print("Activated option:",0,0);
  myGLCD.print(p.text,0,16);
  o.drawn=0;
  delay(1000);
  myGLCD.clrScr();
  return true;
}

int aValue=50;
float fValue=101.1;
/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
MENU(subMenu,"Sub-Menu",
  OP("Op1",menu::nothing),
  OP("Op2",menu::nothing),
  OP("Op3",menu::nothing)
);

/*MENU(menuSetup,"Menu config",
  FIELD(myGLCD.*/

MENU(mainMenu,"Sistema",
  OP("Option A",sayIt),
  OP("Option B",sayIt),
  OP("Option C",sayIt),
  OP("Option D",sayIt),
  OP("Option E",sayIt),
  OP("Option F",sayIt),
  FIELD(aValue,"Value","%",0,100,1,0),
  FIELD(fValue,"Value"," Hz",1,100000,100,0.5),
  SUBMENU(subMenu)
);

void setup() {
  Serial.begin(9600);
  myGLCD.InitLCD();
  myGLCD.setBrightness(4);
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);

  gfx.init();//setup geometry after myGLCD initialized
  //restrict menu area, for scroll and boundary tests
  gfx.maxX=48;
  gfx.maxY=5;
  mainMenu.setPosition(30,20);
  mainMenu.data[1]->enabled=false;
  gfx.bgColor=VGA_GRAY;
}

void loop() {
  mainMenu.poll(gfx,in);
}
