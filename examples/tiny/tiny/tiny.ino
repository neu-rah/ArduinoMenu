// #include <Dump.h>
#include <menu.h>
using namespace AM5;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op x ... reused text!";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText menu_title="Main menu";

//the output description
using Out=FullPrinterDef<
  TextFmt<
    RangePanel<
      StaticPanel<0,0, 10,4,
        SerialOutDef<Serial>
      >
    >
  >
>;

//normal option description
//using flash text
template<typename T,T* text>
using Op=EnDisDef<//with enabled/disabled status
  StaticFlashTextDef<T*,text>
>;

//menu description and content
using MainMenu=StaticFlashTextDef<decltype(&menu_title),&menu_title,
  StaticMenuDataDef<
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

// MainMenu mainMenu;
using Nav=NavDef<Out,MainMenu>;
Nav::Root<Nav::PosDef<>> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 Static demo"<<endl;
  // dumpRam(Serial, op1_text, 16);
  // dumpPgm(Serial, op1_text, 16);
  //disabling some options
  nav.enable<1>(false);
  nav.enable<5>(false);
  Out::raw(F("raw printer test!"));
  Out::nl();
  nav.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '\\':
      nav.enable<5>(!nav.enabled<5>());
      return true;
    case '+': return nav.up();;
    case '-': return nav.down();;
    case '*': return nav.enter();;
    case '/': return nav.esc();;
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) nav.printMenu();
  }
}
