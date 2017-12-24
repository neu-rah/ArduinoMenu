/********************
Arduino generic menu system

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Analog Joystick + Serial
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/analogAxisIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

#ifdef ESP8266
  #define JOY_X 2
  #define JOY_Y 3
  #define JOY_BTN 4
#else
  #define JOY_X A6
  #define JOY_Y A7
  #define JOY_BTN 4
#endif

//analog joystick parameters
// uint8_t pin,
// uint8_t sensivity=5,
// bool inv=false,
// int center=512,
// int inertia=6,
// int bufferSz=1,
// navCmds posCmd=upCmd,
// navCmds negCmd=downCmd
analogAxis<JOY_Y,10,false> ay;

int test=500;

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,FIELD(test,"Test","ms",0,1000,100,1,doNothing,noEvent,wrapStyle)
  ,EXIT("<Back")
);

//joystick button pin
#define joyBtn 4

keyMap btnsMap[]={{-joyBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> btns(btnsMap);// 1 is the number of keys

serialIn menuSerialIn(Serial);

MENU_INPUTS(in,&ay,&btns,&menuSerialIn);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  btns.begin();
  Serial.println("Menu 4.x joysick example");
}

void loop() {
  nav.poll();
  // while(menuSerialIn.available())
  //   Serial.write(menuSerialIn.read());
  delay(100);
}
