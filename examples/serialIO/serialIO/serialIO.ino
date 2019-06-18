//menu system include
#include <menu.h>
//menu output drivers
#include <menu/IO/serialOut.h>
//menu components
#include <menu/comp/endis.h>//options can be enabled/disabled
#include <menu/comp/flashText.h>//use flash text
#include <menu/comp/numField.h>//numeric fields
//menu output formats
#include <menu/fmt/textFmt.h>//default text format
#include <menu/fmt/titleWrap.h>//wrap menu title with []

template<typename O>
using TitleWrap=TitleWrapFmt<O>;//default wrap

using Out=Chain<
  TextFmt,
  TitleWrap,
  FullPrinter,
  Viewport,
  RangePanel
>::To<
  StaticPanel<0,0,80,6,SerialOut<>>
>;

Out out;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText tpt_text="Temp.";
PROGMEM ConstText tpt_unit_text="ºC";
PROGMEM ConstText mainMenu_title="Main menu";
PROGMEM ConstText subMenu_title="Sub-menu";

template<typename T,T* text,typename O=Empty<>>
using Op=EnDis<FlashText<T,text,O>>;

bool hey() {
  Serial.println(F("Hey!"));
  return false;
}

bool grrr() {
  Serial.println(F("This should not be called as the option is disabled"));
  return false;
}

int tpt=25;

using SubMenu=Prompt<
  FlashText<
    decltype(subMenu_title),
    &subMenu_title,
    StaticMenu<
      Op<decltype(op2_text),&op2_text>,
      Op<decltype(op2_text),&op2_text>
    >
  >
>;

using MainMenu=FlashText<
  decltype(mainMenu_title),
  &mainMenu_title,
  StaticMenu<
    Action<Op<decltype(op1_text),&op1_text>,hey>,
    Action<Op<decltype(op2_text),&op2_text>,grrr>,
    Op<
      decltype(tpt_text),
      &tpt_text,
      AsMode<
        AsValue<
          NavHandler<
            NumField<int,tpt,-100,100,10,1,AsUnit<FlashText<decltype(tpt_unit_text),&tpt_unit_text>>>
          >
        >
      >
    >,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    SubMenu,
    Op<decltype(op3_text),&op3_text>
  >
>;

Chain<
  StaticNav,
  TinyNavPos,
  NavBase,
  Drift
>::To<MainMenu> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 serial example"));
  nav.enable(1,false);//disable second option
  nav.printMenu(out);
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
    if (keys(Serial.read())) nav.printMenu(out);
  }
}
