#include <menu.h>
#include <menu/IO/arduino/printOut.h>
#include <menu/IO/arduino/streamIn.h>
#include <menu/comp/arduino/flashText.h>
#include <menu/fmt/text.h>
#include "fpsTicks.h"

using Out=MenuOut<
  FullPrinter<
    TextFmt<
      RangePanel<
        StaticPanel<0,0,20,4,ArduinoPrintOut<>>
      >
    >
  >
>;

Out out;

const char op1_txt[] PROGMEM="Option 1";
const char op2_txt[] PROGMEM="Option 2";
const char op3_txt[] PROGMEM="...";
const char main_txt[] PROGMEM="Main menu";
const char sub_txt[] PROGMEM="Sub-menu";
using Op1=FlashText<decltype(op1_txt),&op1_txt>;
using Op2=FlashText<decltype(op2_txt),&op2_txt>;
using Op3=FlashText<decltype(op3_txt),&op3_txt>;

bool test() {
  Serial.println(F("Test action called!"));
  return true;
}

bool action1() {
  Serial.println(F("action 1!"));
  return true;
}

bool action2() {
  Serial.println(F("action 1!"));
  return true;
}

using MainMenu=StaticMenu<
  FlashText<decltype(main_txt),&main_txt>,
  StaticData<
    Action<Op1,action1>,
    Action<Op2,action2>,
    StaticMenu<
      FlashText<decltype(sub_txt),&sub_txt>,
      StaticData<Action<Op1,test>,Action<Op2,test>,Op3>
    >,
    Action<Op3,test>
  >
>;

MainMenu mainMenu;

StaticRoot<
  StaticNavTree<MainMenu,2,NavPos<>>
> nav(mainMenu);

using SerialIn=ArduinoStreamIn<>;

FPS<25> menuFps;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 AVR Test."));
  Serial.println(F("This is a test for size, expect minimalist menu"));
  nav.print<Out>();
}

void loop() {
  if (menuFps&&nav.doInput<SerialIn>()) nav.print<Out>();
}
