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

using MainMenu=StaticMenu<
  FlashText<decltype(main_txt),&main_txt>,
  StaticData<
    Op1,
    Op2,
    Op3,
    StaticMenu<
      FlashText<decltype(sub_txt),&sub_txt>,
      StaticData<Op1,Op2,Op3>
    >,
    Op3
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
