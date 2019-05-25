////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu lcd+serial output chain example

#include <menu.h>
#include <menu/IO/liquidCrystalOut.h>
#include <menu/IO/serialOut.h>
#include <menu/IO/outList.h>
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/textFmt.h>
#include <menu/comp/flashText.h>

// using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

template<typename O>
using TitleWrap=TitleWrapFmt<O>;//default wrap

template<typename O>
using TextPanel=Chain<
  TextFmt,
  TitleWrap,
  FullPrinter,
  Viewport,
  RangePanel
>::To<O>;

//dual head output
using Out=OutList<
  TextPanel<StaticPanel<0,0,16,2,LiquidCrystalOut<lcd>>>,
  TextPanel<StaticPanel<0,0,80,5,SerialOut<>>>
>;

//string data on flash
PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op ...";
PROGMEM ConstText op3_text="Op 3";
PROGMEM ConstText year_text="Year";
PROGMEM ConstText mainMenu_title="Main menu";

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

int year=2019;

using MainMenu=
  FlashText<
    decltype(mainMenu_title),
    &mainMenu_title,
    StaticMenu<
      // Action<Op<decltype(op1_text),&op1_text>,hey>,
      // Action<Op<decltype(op2_text),&op2_text>,grrr>,
      Op<
        decltype(year_text),
        &year_text,
        NumField<
          int,
          year,
          1900,2100,
          10,1,
          AsUnit<Empty<>>
        >
      >
      // Op<decltype(op2_text),&op2_text>,
      // Op<decltype(op2_text),&op2_text>,
      // Op<decltype(op2_text),&op2_text>,
      // Op<decltype(op2_text),&op2_text>,
      // Op<decltype(op3_text),&op3_text>
    >
  >;

NavCap<StaticNav<Out,MainMenu,NavPos<>>> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 LiquidCrystal example");
  lcd.begin(16,2);
  nav.enable(1,false);
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
