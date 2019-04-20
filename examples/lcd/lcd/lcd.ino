#include <menu.h>
#include <menu/fmt/text.h>
using namespace AM5;

//not working yet!!!

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

using Out=RangePanel<
  StaticPanel<0,0,16,2.
    LCDOut<lcd>
  >
>;
