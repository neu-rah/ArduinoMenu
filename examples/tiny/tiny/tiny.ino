/////////////////////////////////////////////////////
// ArduinoMenu libtary 5.x code example
// Output: none (menu in headless mode)
// Input: user handled (arduino serial)
//
// this is a minimalistic menu, no texts or options lists
// only a size and a position (zero indexed)
//
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)

/* Footprint on ATTiny13 https://github.com/MCUdude/MicroCore
Sketch uses 456 bytes (44%) of program storage space. Maximum is 1024 bytes.
Global variables use 11 bytes (17%) of dynamic memory, leaving 53 bytes for local variables. Maximum is 64 bytes.
*/

#include <menu.h>
#include <menu/nav.h>

using namespace AM5;

SelfNav<EmptyMenu<4>> mainMenu;

#define ATTINY13
#ifdef ATTINY13
  #define BTN_UP 0// Up
  #define BTN_DOWN 1 // Down
#else
  // #define BTN_SEL 8  // Select button
  #define BTN_UP 7 // Up
  #define BTN_DOWN A5 // Down
#endif

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BTN_UP,INPUT_PULLUP);
  pinMode(BTN_DOWN,INPUT_PULLUP);
}

unsigned int dutty=0;
unsigned int cycle=0;

void actions() {
  switch(mainMenu.pos()) {
    case 0:
      dutty=0;
      cycle=0;
      break;
    case 1:
      dutty=10;
      cycle=100;
      break;
    case 2:
      dutty=90;
      cycle=100;
      break;
    case 3:
      dutty=100;
      cycle=1000;
      break;
  }
}

//blink a boolean without delay
inline bool blinker(unsigned int dutty,unsigned int cycle) {return millis()%cycle<dutty;}

void loop() {
  if (!digitalRead(BTN_UP)) {
    mainMenu.up();
    actions();
  } else if (!digitalRead(BTN_DOWN)) {
    mainMenu.down();
    actions();
  }
  digitalWrite(LED_BUILTIN, blinker(dutty,cycle));
}
