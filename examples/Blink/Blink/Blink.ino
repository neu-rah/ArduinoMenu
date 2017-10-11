/********************
Arduino generic menu system
control led on/off delays

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial
mcu: nano328p
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN
#define MAX_DEPTH 1

int timeOn=10;
int timeOff=90;

MENU(mainMenu, "Blink menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,100,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

serialIn serial(Serial);
MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}
