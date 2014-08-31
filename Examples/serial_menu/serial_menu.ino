#include "menu.h"//menu macros and objects
#include "menuPrint.h"

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

void none() {}
void ledOn() {digitalWrite(13,1);}
void ledOff() {digitalWrite(13,0);}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
MENU(mainMenu,"Main menu",
  OP("LED On",ledOn),
  OP("LED Off",ledOff),
  OP("Disabled option",none)
);

menuPrint menu_out(Serial);//describe output device

void setup() {
  Serial.begin(9600);
  Serial.println("menu system test");
  pinMode(13,OUTPUT);
  menu::exit="Saida";
  mainMenu.data[2]->enabled=false;
}

void loop() {
  mainMenu.activate(menu_out,Serial);
  Serial.println("");
  Serial.println("Restarting...");
}
