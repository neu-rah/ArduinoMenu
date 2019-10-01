/********************
Arduino generic menu system

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial
*/

#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <menuIO/uartOut.h>
#include <SoftwareSerial.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

SoftwareSerial ss(11, 10);


#define MAX_DEPTH 2

// define menu colors --------------------------------------------------------
#define C_BLACK 0
#define C_BLUE 3
#define C_GRAY 7
#define C_WHITE 15
#define C_YELLOW 31
#define C_RED 1
#define C_GREEN 2
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint8_t> colors[6] MEMMODE={
  {{C_BLACK,C_BLACK}, {C_BLACK,C_BLUE,C_BLUE}},//bgColor
  {{C_GRAY,C_GRAY},   {C_WHITE,C_WHITE,C_WHITE}},//fgColor
  {{C_WHITE,C_BLACK}, {C_YELLOW,C_YELLOW,C_RED}},//valColor
  {{C_WHITE,C_BLACK}, {C_WHITE,C_YELLOW,C_YELLOW}},//unitColor
  {{C_WHITE,C_GRAY},  {C_BLACK,C_BLUE,C_WHITE}},//cursorColor
  {{C_WHITE,C_YELLOW},{C_GREEN,C_WHITE,C_WHITE}},//titleColor
};


#define offsetX 0
#define offsetY 0
#define U8_Width 240
#define U8_Height 320

#define LEDPIN LED_BUILTIN


int timeOn=10;
int timeOff=90;

MENU(mainMenu, "Settings menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(timeOn,"On","ms",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(timeOff,"Off","ms",0,100,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("<Back")
);

serialIn serial(Serial);

MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,UART_OUT(ss,colors,uartOut::systemFont::font24,offsetX,offsetY,{0,0,U8_Width/uartOut::systemFont::font24*2,U8_Height/uartOut::systemFont::font24})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  ss.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
}

void loop() {
  nav.doInput();
  if (nav.changed(0)) {
    //only draw if menu changed for gfx device
    nav.doOutput();
  }
  digitalWrite(LEDPIN, HIGH);
  delay(timeOn);
  digitalWrite(LEDPIN, LOW);
  delay(timeOff);
}

