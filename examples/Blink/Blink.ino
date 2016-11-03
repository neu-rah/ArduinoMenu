#include <Arduino.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>

#define LEDPIN 13
#define MAX_DEPTH 1

int timeOn=10;
int timeOff=90;

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,500,100,10, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,500,100,10,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

MENU_INPUTS(in,&Serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

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
