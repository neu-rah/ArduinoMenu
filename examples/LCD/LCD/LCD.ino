#include <menu.h>
using namespace AM5;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op x ... reused text!";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText menu_title="Main menu";

//the output description
using Out=FullPrinterDef<
  RangePanel<
    Viewport<
      StaticPanel<0,0, 10,4,LCDOut<lcd>>
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

using Nav=NavDef<Out,MainMenu>;
using NavRoot=Nav::Root<Nav::PosDef<>>;
NavRoot nav;

template<typename I>
NavRoot& operator<<(NavRoot& o,I i) {o.template raw<NavRoot,I,nav>(i);return o;}

NavRoot& operator<<(NavRoot& o,endlObj&) {o.template nl<NavRoot,nav>();return o;}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  nav<<F("AM5 example ---")<<endl;
  delay(1500);
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
