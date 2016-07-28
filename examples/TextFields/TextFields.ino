/********************

THIS IS EXPERIMENTAL


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
#define DEBUG
#include <UTFT.h>
#include <UTouch.h>
#ifdef DEBUG
//#include <streamFlow.h>
#endif
#include <menu.h>
#include <menuUTFT.h>
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <menuUTouch.h>
#include <menuFields.h>
#include <controls/common/menuTextFields.h>
#include <menuPrint.h>

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
  return false;
}

int aValue=50;
float fValue=101.1;
/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it

/*MENU(menuSetup,"Menu config",
  FIELD(myGLCD.*/

char name[20+1]="                    ";

bool updateName(prompt& p,menuOut& o,Stream &c) {
  o.println(name);
  return false;
}

MENU(subMenu,"Sub-menu",
  OP("Test 1",sayIt),
  OP("Test 2",sayIt)
);

MENU(mainMenu,"Arduino Menu",
  TEXTFIELD(name,"Name",updateName),
  OP("Option A...",sayIt),
  SUBMENU(subMenu),
  FIELD(aValue,"Value","%",0,100,1,0)
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
  //mainMenu.data[1]->enabled=false;
  gfx.bgColor=VGA_GRAY;
  name[2]='X';
  Serial.print("char size is:");Serial.println(sizeof(char));
  pinMode(13,OUTPUT);
}

//menuPrint menuSerial(Serial);//describe output device

void loop() {
  mainMenu.poll(gfx,in);
  //mainMenu.poll(menuSerial,in);
  digitalWrite(13,(millis()>>8)&1);
}
