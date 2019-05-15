#include <menu.h>
#include <menu/IO/liquidCrystalOut.h>
#include <menu/comp/endis.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/textFmt.h>
#include <menu/comp/flashText.h>

// using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

using Out=
  TextFmt<
    TitleWrap<
      LiquidCrystalOut<
        lcd,
        Viewport<
          RangePanel<
            StaticPanel<0,0,16,2,
              FullPrinter<>
            >
          >
        >
      >
    >
  >;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText mainMenu_title="Main menu";

template<typename T,T* text>
using Op=EnDis<FlashText<T,text>>;

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

NavCap<StaticNav<Out,MainMenu,NavPos<>>> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 LiquidCrystal example");
  lcd.begin(16,2);
  nav.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '+': return nav.up();
    case '-': return nav.down();
    case '*': return nav.enter();
    case '/': return nav.esc();
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())||true) nav.printMenu();
  }
}
