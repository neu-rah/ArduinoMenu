////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu serial output example

#include <menu.h>
#include <menu/IO/serialOut.h>
// #include <menu/comp/endis.h>
#include <menu/comp/flashText.h>
#include <menu/comp/numField.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/textFmt.h>

// using namespace Menu;

template<typename O>
using TitleWrap=TitleWrapFmt<O>;//default wrap

using Out=Chain<
  TextFmt,
  TitleWrap,
  FullPrinter
>::To<
  SerialOut<>
>;

//the above `Chain` is equivalent to this:
// using Out=
//   TextFmt<
//     TitleWrap<
//       FullPrinter<SerialOut<>>
//     >
//   >;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
// PROGMEM ConstText year_text="Year";
PROGMEM ConstText mainMenu_title="Main menu";

template<typename T,T* text,ActionHandler act=doNothing>
using Op=Action<FlashText<T,text>,act>;

bool hey() {
  Serial.println(F("Hey!"));
  return false;
}

bool grrr() {
  Serial.println(F("This should not be called as the option is disabled"));
  return false;
}

int year=2019;

using MainMenu=
  FlashText<
    decltype(mainMenu_title),
    &mainMenu_title,
    StaticMenu<
      Op<decltype(op1_text),&op1_text,hey>,
      Op<decltype(op2_text),&op2_text,grrr>,
      Op<decltype(op3_text),&op3_text>
    >
  >;

NavRoot<
  ItemNav<
    StaticNav<Out,MainMenu,NavPos<>>
  >
> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 serial example");
  nav.printMenu();
  // nav.enable(1,false);//no effect as we are not using en/dis options
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '\\':
      //dynamic toggle option[6] enabled state
      nav.enable(5,!nav.enabled(5));
      return true;
    case '+': return nav.up();
    case '-': return nav.down();
    case '*': return nav.enter();
    case '/': return nav.esc();
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) nav.printMenu();
  }
}
