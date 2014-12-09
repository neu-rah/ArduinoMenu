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

int param;

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions
  
void none() {}
/*void ledOn() {digitalWrite(13,1);}
void ledOff() {digitalWrite(13,0);}*/

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
bool test=false;
TOGGLE(test,onoff_tog,"led: ",
  VALUE("On",HIGH),
  VALUE("Off",LOW)
);


MENU(mainMenu,"Main menu",
  /*OP("LED On",ledOn),
  OP("LED Off",ledOff),*/
  OP("Disabled option",none),
  SUBMENU(onoff_tog),
  FIELD(param,"Name","%",0,100,10,1)
);

menuPrint menu_out(Serial);//describe output device

void setup() {
  Serial.begin(9600);
  Serial.println("menu system test");
  pinMode(13,OUTPUT);
  menu::exit="Sair";
  mainMenu.data[0]->enabled=false;
}

void loop() {
  mainMenu.poll(menu_out,Serial);
  digitalWrite(13,test);
}
