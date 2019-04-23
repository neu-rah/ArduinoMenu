#include <streamFlow.h>
#include <menu.h>
#include <menu/fmt/text.h>
#include <menu/comp/flashText.h>//flash text components
#include <menu/IO/lcdOut.h>//serial output
#include <menu/IO/serialOut.h>//serial output
using namespace AM5;

//not working yet!!!

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

using MyLcdOut=TextFmt<
  Viewport<
    RangePanel<
      StaticPanel<0,0,16,2,
        LCDOut<lcd>
      >
    >
  >
>;

using MySerialOut=TextFmt<SerialOut<>>;

using Out=OutList<
  MyLcdOut,
  MySerialOut
>;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText menu_title="Main menu";

template<typename T,T* text>
using Op=EnDis<FlashText<T,text>>;

using Menu=FlashText<decltype(menu_title),&menu_title,
  StaticMenu<
    Op<decltype(op1_text),&op1_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op2_text),&op2_text>,
    Op<decltype(op3_text),&op3_text>
  >
>;

using NavRoot=Nav<
  Menu,
  Out,
  PosDef<>
>;
NavRoot nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  nav<<"AM5 example"<<nav.endl;
  nav.enable<1>(false);
  nav.enable<5>(false);
  nav.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '\\':
      //dynamic toggle option[6] enabled state
      nav.enable<5>(!nav.enabled<5>());
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
