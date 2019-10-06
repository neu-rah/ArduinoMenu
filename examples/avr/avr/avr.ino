#include <menu.h>
#include <menu/IO/serialOut.h>
#include <menu/fmt/text.h>
#include "fpsTicks.h"

using Out=MenuOut<
  FullPrinter<
    TextFmt<
      RangePanel<
        StaticPanel<0,0,20,4,ArduinoSerialOut<>>
      >
    >
  >
>;

Out out;

const char* op1_txt="Option 1";
const char* op2_txt="Option 2";
const char* op3_txt="...";
const char* main_txt="Main menu";
const char* sub_txt="Sub-menu";
using Op1=StaticText<&op1_txt>;
using Op2=StaticText<&op2_txt>;
using Op3=StaticText<&op3_txt>;

using MainMenu=StaticMenu<
  StaticText<&main_txt>,
  StaticData<
    Op1,
    Op2,
    Op3,
    Op3,
    Op3,
    StaticMenu<
      StaticText<&sub_txt>,
      StaticData<Op1,Op2,Op3>
    >,
    Op3
  >
>;

MainMenu mainMenu;

StaticRoot<
  StaticNavTree<MainMenu,2,NavPos<>>
> nav(mainMenu);

template<typename Dev=decltype(Serial), Dev& dev=Serial,typename O=Void<>>
struct ArduinoStreamIn:O {
  template<typename Nav,bool invY=false>
  inline static bool cmd(Nav& nav) {
    // _trace(MDO<<"StreamIn"<<endl);
    if (dev.available()) {
      switch(dev.read()) {
        case '+': return invY?nav.down():nav.up();
        case '-': return invY?nav.up():nav.down();
        case '*': return nav.enter();
        case '/': return nav.esc();
        default: break;
      }
    }
    return false;
  }
};

using SerialIn=ArduinoStreamIn<>;

FPS<5> menuFps;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 AVR Test.");
  Serial.println("This is a test for size, expect minimalist menu");
  nav.print<Out>();
}

void loop() {
  if (menuFps&&nav.doInput<SerialIn>()) nav.print<Out>();
}
