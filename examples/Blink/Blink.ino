#include <Arduino.h>
#include <menu.h>
#include <dev/serialOut.h>
#include <dev/chainStream.h>

#define LEDPIN 13
#define MAX_DEPT 1

int timeOn=500;
int timeOff=500;

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,100,10, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,1000,100,10,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

MENU_INPUTS(in,&Serial);

Menu::idx_t tops[MAX_DEPT];
Menu::serialOut outSerial(Serial,tops);//,serial_panels);//the output device (just the serial port)

MENU_OUTPUTS(out,&outSerial);
NAVROOT(nav,mainMenu,MAX_DEPT,in,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 3.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}
