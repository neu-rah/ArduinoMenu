/*
This was just compiled.. not uploaded it to an attiny 13 yet...
the purpose was just to check memory contraints
upload at own risk ;)
*/

/* memory footprints

with idx_t=int8_t
DATA:    [=======   ]  65.6% (used 42 bytes from 64 bytes)
PROGRAM: [======    ]  57.8% (used 592 bytes from 1024 bytes)

with idx_t=int
DATA:    [========= ]  89.1% (used 57 bytes from 64 bytes)
PROGRAM: [=======   ]  68.6% (used 702 bytes from 1024 bytes)

with idx_t=int and using TinyNavPos (insignificant, 2 bytes per menu)
DATA:    [========= ]  85.9% (used 55 bytes from 64 bytes)
PROGRAM: [======    ]  64.5% (used 660 bytes from 1024 bytes)
*/

#include <menu.h>

#include <menu/IO/serialOut.h>

#include <menu/comp/endis.h>
#include <menu/comp/flashText.h>
#include <menu/comp/numField.h>

#include <menu/fmt/textFmt.h>
#include <menu/fmt/titleWrap.h>

template<typename O>
using TitleWrap=TitleWrapFmt<O>;//default wrap

using Out=Chain<
  // TextFmt,
  // TitleWrap,
  FullPrinter
>::To<SerialOut<>>;

Out out;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op 2";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText mainMenu_title="Main menu";

template<typename T,T* text,typename O=Empty<>>
using Op=FlashText<T,text>;

using MainMenu=
  FlashText<
    decltype(mainMenu_title),
    &mainMenu_title,
    StaticMenu<
      Op<decltype(op1_text),&op1_text>,
      Op<decltype(op2_text),&op2_text>,
      Op<decltype(op3_text),&op3_text>
    >
  >;

Chain<
  StaticNav,
  TinyNavPos,
  NavBase,
  Drift
>::To<MainMenu> nav;


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

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 serial example"));
  nav.enable(1,false);//disable second option
  nav.printMenu(out);
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) nav.printMenu(out);
  }
}
