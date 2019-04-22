#include <streamFlow.h>
#include <menu.h>//ArduinoMenu main include
#include <menu/fmt/text.h>//get some text format
#include <menu/comp/flashText.h>//flash text components
#include <menu/IO/serialOut.h>//serial output
using namespace AM5;

//define an output type
using Out=TextFmt<
  RangePanel<
    //activate this panel to limit output size on serial console
    //unlimited by default
    // StaticPanel<0,0,10,5,
      SerialOut<>
    // >
  >
>;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText menu_title="Main menu";

//normal option... can mix others
template<typename T,T* text>
using Op=EnDis<FlashText<T,text>>;

//menu structure type
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

//navigation control
using NavRoot=Nav<Menu, Out, PosDef<>>;
NavRoot nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
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
