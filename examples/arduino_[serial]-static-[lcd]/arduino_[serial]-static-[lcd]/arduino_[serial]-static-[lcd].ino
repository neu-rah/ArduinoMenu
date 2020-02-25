////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu lcd+serial output chain example

#include <staticMenu.h>
#include <menu/IO/Arduino/liquidCrystalOut.h>
#include <menu/IO/Arduino/serialIn.h>
// #include <menu/IO/serialOut.h>
// #include <menu/IO/outList.h>
#include <menu/comp/endis.h>
// #include <menu/comp/numField.h>
#include <menu/fmt/titleWrap.h>
#include <menu/fmt/fullText.h>
#include <menu/comp/Arduino/flashText.h>
using namespace Menu;

// using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//string data on flash
extern const char mainMenu_title[] PROGMEM="Main menu";
extern const char subMenu_title[] PROGMEM="Sub-menu";
extern const char op1_text[] PROGMEM="Option 1";
extern const char op2_text[] PROGMEM="Option 2";
extern const char opn_text[] PROGMEM="Op...";
extern const char exit_text[] PROGMEM="<Exit";

bool hey() {
  Serial.println(F("Hey!"));
  return false;
}

bool grrr() {
  Serial.println(F("This should not be called as the option is disabled"));
  return false;
}

int tpt=25;

Item<
  Mutable::Part,
  StaticMenu<
    Item<Mutable::Part,FlashText<decltype(mainMenu_title),&mainMenu_title>::Part>,
    StaticData<
      Item<Action<hey>::Part,Mutable::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
      Item<Action<grrr>::Part,Mutable::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
      Item<
        StaticMenu<
          Item<Mutable::Part,FlashText<decltype(subMenu_title),&subMenu_title>::Part>,
          StaticData<
            Item<Mutable::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<Mutable::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<Mutable::Part,FlashText<decltype(exit_text),&exit_text>::Part>
          >
        >::Part
      >,
      Item<EnDis<false>::Part,Action<grrr>::Part,Mutable::Part,FlashText<decltype(opn_text),&opn_text>::Part>
    >
  >::Part
> mainMenu;

//menu input --------------------------------------
SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

//dual head output
// using Out=OutList<
//   TextPanel<StaticPanel<0,0,16,2,LiquidCrystalOut<lcd>>>,
//   TextPanel<StaticPanel<0,0,80,6,SerialOut<>>>
// >;

//menu output (Serial)
using Out=StaticMenuOut<
  FullPrinter::Part,//print title and items
  TitleWrapFmt<>::Part,//put [] around menu title
  TextFmt::Part,//apply text formating
  PartialDraw::Part,//this device can position the cursor and do a partial draw
  PanelTarget::Part,//detect target (menu) changes
  RangePanel<>::Part,//control vertical scrolling
  StaticPanel<0,0,16,2>::Part,//define output geometry
  LiquidCrystalOut<lcd>::Part//raw output device
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
StaticNavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("AM5 LiquidCrystal example"));
  lcd.begin(16,2);
  nav.print(out);
}

void loop() {
  if (Serial.available()) {
    if (nav.doInput(in)) nav.print(out);
  }
}
