/***
   author: Rui Azevedo (neu-rah) Feb 2020
  example: arduino (generic)
     menu: static
   output: I2C LCD
    input:
tested on: nano
*/

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//main include for ArduinoMenu
#include <staticMenu.h>
//input/output drivers --------------------------------------
#include <menu/IO/Arduino/i2cLCDOut.h>
#include <menu/IO/Arduino/serialIn.h>
// #include <menu/IO/Arduino/serialOut.h>
// #include <menu/IO/Arduino/serialIO.h>//include both serial in and out
//format specifyers -----------------------------------------
#include <menu/fmt/textEditMode.h>//edit mode cursor
#include <menu/fmt/textCursor.h>//slected option cursor
#include <menu/fmt/textItem.h>//cursor moves 1 line on each item
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
// #include <menu/comp/numField.h>//numeric fields with name, value and unit
#include <menu/comp/Arduino/flashText.h>
using namespace Menu;

//------------------------------
//menu action handlers
bool tog12();
#define BTN A0

extern const char mainMenu_title[] PROGMEM="Main menu";
extern const char subMenu_title[] PROGMEM="Sub-menu";
extern const char op1_text[] PROGMEM="Option 1";
extern const char op2_text[] PROGMEM="Option 2";
extern const char tog12_text[] PROGMEM="toggle 1&2";
extern const char opn_text[] PROGMEM="Option ...";
extern const char exit_txt[] PROGMEM="<Exit";
// extern const char yr_txt[] PROGMEM="Year";
// extern const char vcc_txt[] PROGMEM="VCC";
// extern const char volts_txt[] PROGMEM="V";

//static menu structure
Item<
  StaticMenu<
    FlashText<decltype(mainMenu_title),&mainMenu_title>::Part<>,
    StaticData<
      Item<EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
      Item<EnDis<false>::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
      Item<Action<tog12>::Part,FlashText<decltype(tog12_text),&tog12_text>::Part>,
      Item<FlashText<decltype(opn_text),&opn_text>::Part>,
      Item<
        StaticMenu<
          FlashText<decltype(subMenu_title),&subMenu_title>::Part<>,
          StaticData<
            Item<EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
            Item<FlashText<decltype(op2_text),&op2_text>::Part>,
            Item<FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<FlashText<decltype(exit_txt),&exit_txt>::Part>
          >
        >::Part
      >
    >
  >::Part
> mainMenu;//create menu object

//menu input --------------------------------------
SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

//menu output (Serial)
using Out=StaticMenuOut<
  FullPrinter::Part,//print title and items
  TitleWrapFmt<>::Part,//put [] around menu title
  TextCursorFmt,//draw text cursor
  TextEditModeFmt,//draw edit mode text cursor
  TextItemFmt,//item on a new line
  PartialDraw::Part,//this device can position the cursor and do a partial draw
  PanelTarget::Part,//track target (menu) change
  RangePanel<>::Part,//control vertical scrolling
  StaticPanel<0,0,16,2>::Part,//define output geometry
  I2CLCDOut<lcd>::Part,//raw output device
  TextMeasure<>::Part//default monometric text measure
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
StaticNavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

//menu action handlers implementation
bool tog12() {
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 I2C LCD example");Serial.flush();
  lcd.begin(16,2);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.print("AM5 I2C LCD");
  delay(1000);
  lcd.clear();
  Serial.println("DEBUG");Serial.flush();
  nav.print(out);//initial menu draw
}

// long nextMenuFrame=MENURATE;
void loop(){
  if (nav.doInput(in)) nav.print(out);
  delay(100);
}
