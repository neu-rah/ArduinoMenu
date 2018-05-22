// still experimental!
// using
//   http://playground.arduino.cc/Code/Keypad
//   https://github.com/Nullkraft/Keypad

#include <menu.h>//menu macros and objects
#include <menuIO/keypadIn.h>
#include <menuIO/serialOut.h>

using namespace Menu;

//define your keyboard ------------------------------------
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','-'},
  {'/','0','*','+'},
};
byte rowPins[ROWS] = {6,7,8,9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2,3,4,5}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//define a menu -----------------------------------------------------
char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";//<-- menu will edit this text

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,SUBMENU(chooseMenu)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE
);

keypadIn kpad(customKeypad);
NAVROOT(nav,mainMenu,MAX_DEPTH,kpad,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x keypad example");
  delay(1000);
}

void loop() {
  nav.poll();
  delay(100);
}
