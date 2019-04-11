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
#include <menu/comp/flashMenu.h>

using namespace AM5;

#define ATTINY13
#ifdef ATTINY13
  #define BTN_UP 0// Up
  #define BTN_DOWN 1 // Down
  // #define LED_BUILTIN 2
#else
  // #define BTN_SEL 8  // Select button
  #define BTN_UP 7 // Up
  #define BTN_DOWN A5 // Down
#endif

unsigned int dutty=0;
unsigned int cycle=0;

template<ActionHandler act=doNothing>
using Op=Prompt<Action<Empty,act>>;

bool off() {
  dutty=0;
  cycle=0;
  return false;
}

bool low_speed() {
  dutty=10;
  cycle=100;
  return false;
}

bool high_speed() {
  dutty=90;
  cycle=100;
  return false;
}

Op<off> op_off;
Op<low_speed> op_low;
Op<high_speed> op_high;

constexpr FlashData data[3] {&op_off,&op_low,&op_high};
Prompt<FlashMenuDef<data,3,Empty>> mainMenu;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BTN_UP,INPUT_PULLUP);
  pinMode(BTN_DOWN,INPUT_PULLUP);
}

//blink a boolean without delay
inline bool blinker(unsigned int dutty,unsigned int cycle) {return millis()%cycle<dutty;}

void loop() {
  if (!digitalRead(BTN_UP)) mainMenu.up();
  else if (!digitalRead(BTN_DOWN)) mainMenu.down();
  digitalWrite(LED_BUILTIN, blinker(dutty,cycle));
}
