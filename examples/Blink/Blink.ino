#include <Arduino.h>
#include <menu.h>
#include <dev/serialOut.h>

#define LEDPIN 13

int timeOn=500;
int timeOff=500;

MENU(mainMenu, "Blink menu", doNothing, noEvent, wrapStyle
  ,FIELD(timeOn,"On","ms",0,1000,100,10,doNothing,noEvent,noStyle)
  ,FIELD(timeOff,"Off","ms",0,1000,100,10,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

MENU_INPUTS(in,&Serial);

serialOut outSerial(Serial);//,serial_panels);//the output device (just the serial port)

MENU_OUTPUTS(out,&outSerial);
NAVROOT(nav,mainMenu,1,in,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}
