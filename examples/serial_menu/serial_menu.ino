/********************
Arduino generic menu system
Serial menu example
http://www.r-site.net/?at=//op%5B%40id=%273090%27%5D

Sept.2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes
*/
#include <menu.h>//menu macros and objects
#include <menuFields.h>
#include <menuPrint.h>
//#include <streamFlow.h>

#define LEDPIN 13

//aux vars
int ledCtrl=0;
int percent;//just testing changing this var
double fps=0;
int counter=0;

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions
bool ledOn() {
  Serial.println("set led on!");
  digitalWrite(LEDPIN,ledCtrl=1);
  return false;
}

bool ledOff() {
  Serial.println("set led off!");
  digitalWrite(LEDPIN,ledCtrl=0);
  return false;
}

bool quit() {
  Serial.println("Quiting after action call");
  return true;
}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
// empty options are just for scroll testing

/*bool setLed() {
  digitalWrite(LEDPIN,ledCtrl);
  return false;
}*/
TOGGLE(ledCtrl,setLed,"Led: ",
    VALUE("On",HIGH,ledOn),
    VALUE("Off",LOW,ledOff)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",
  VALUE("Zero",0),
  VALUE("One",1),
  VALUE("Two",2)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose ",
  VALUE("First",1),
  VALUE("Second",2),
  VALUE("Third",3),
  VALUE("Last",-1)
);

MENU(subMenu,"SubMenu"
  ,OP("A",quit)
  ,OP("B",quit)
  ,OP("C",quit)
  ,OP("D",quit)
  ,OP("E",quit)
  ,OP("F",quit)
  ,OP("G",quit)
  ,OP("H",quit)
);

MENU(mainMenu,"Main menu",
  SUBMENU(setLed),
  OP("LED On",ledOn),
  OP("LED Off",ledOff),
  SUBMENU(selMenu),
  SUBMENU(chooseMenu),
  SUBMENU(subMenu),
  FIELD(percent,"Percent","%",0,100,10,1)
);

menuPrint menu_out(Serial);//describe output device

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu system test");
  pinMode(LEDPIN,OUTPUT);
  mainMenu.data[0]->enabled=false;
}

void loop() {
  mainMenu.poll(menu_out,Serial);
  digitalWrite(LEDPIN,ledCtrl);
}
