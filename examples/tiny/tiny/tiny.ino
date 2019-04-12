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

this example is not realy useful, its ajust a navigation state
one can do the same with little effort, no need for a menu.

it stands just as a demo of dropping functionalities available with AM5
*/

#include <menu.h>
#include <menu/nav.h>

using namespace AM5;

//this is the whole definition, just a navigation with 4 states
//no options, no texts, no actions.
// SelfNav<EmptyMenu<4>> mainMenu;

#define ATTINY
#ifdef ATTINY
  #define LED_BUILTIN 5
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

inline bool off() {
  dutty=0;
  cycle=0;
  return false;
}

inline bool low_speed() {
  dutty=10;
  cycle=100;
  return false;
}

inline bool high_speed() {
  dutty=90;
  cycle=100;
  return false;
}

// void actions() {
//   switch(mainMenu.pos()) {
//     case 0:
//       dutty=0;
//       cycle=0;
//       break;
//     case 1:
//       dutty=10;
//       cycle=100;
//       break;
//     case 2:
//       dutty=90;
//       cycle=100;
//       break;
//     case 3:
//       dutty=100;
//       cycle=1000;
//       break;
//   }
// }

template<ActionHandler act=doNothing>
using Op=Prompt<Action<Empty,act>>;

NavNode<
  SelfNav<
    FooMenu<
      Op<off>,
      Op<low_speed>,
      Op<high_speed>
    >
  >
> mainMenu;

//blink a boolean without delay
inline bool blinker(unsigned int dutty,unsigned int cycle) {return millis()%cycle<dutty;}

void loop() {
  if (!digitalRead(BTN_UP)) {
    mainMenu.up();
    // actions();
  } else if (!digitalRead(BTN_DOWN)) {
    mainMenu.down();
    // actions();
  }
  digitalWrite(LED_BUILTIN, blinker(dutty,cycle));
}
