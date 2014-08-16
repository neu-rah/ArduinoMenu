#include <LiquidCrystal.h>
#include "pcint.h"//this is incompatible with software serial (arduino needs an handler!)
#include "menu.h"//menu macros and objects

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions

void setValue(int &value,String text) {
  Serial.println("");
  while(Serial.available()) Serial.read();//remove any unwanted characters
  Serial.print(text);
  while(!Serial.available()) delay(100);
  value=Serial.parseInt();
  Serial.println(value);
  while(Serial.available()) Serial.read();//remove any unwanted characters
}

void ledOn() {digitalWrite(13,1);}
void ledOff() {digitalWrite(13,0);}

int frequency=100;
void setFreq() {setValue(frequency,"Freqquency (Hz):");}

int dutty=50;
void setDutty() {setValue(dutty,"Dutty cycle %:");}

/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
MENU(subMenu,"LED on pin 13",
  OP("LED On",ledOn),
  OP("LED Off",ledOff)
);

MENU(mainMenu,"Sistema",
  OP("Frequency",setFreq),
  OP("Dutty",setDutty),
  SUBMENU(subMenu)
);z

///////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("menu system test");
  pinMode(13,OUTPUT);
}

///////////////////////////////////////////////////////////////////////////////
// testing the menu system
void loop() {
  mainMenu.activate(Serial,Serial);//show menu to Serial and read keys from Serial
  Serial.println("");
  Serial.println("Restarting...");
  Serial.println("-------------");
  Serial.println("");
}
