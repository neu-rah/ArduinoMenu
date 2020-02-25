/***
   author: Rui Azevedo (neu-rah) Feb 2020
  example: aeduino multi-head output, simultaneous on LCD and serial
     menu: static
   output: I2C LCD + Serial
    input: Serial
tested on: nano
*/

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//main include for ArduinoMenu
#include <staticMenu.h>
//input/output drivers --------------------------------------
#include <menu/IO/Arduino/i2cLCDOut.h>
#include <menu/IO/Arduino/ANSIMonitor.h>
#include <menu/IO/Arduino/serialIO.h>
#include <menu/IO/outList.h>
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>//full text format
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

extern const char mainMenu_title[] PROGMEM="Main menu";
extern const char subMenu_title[] PROGMEM="Sub-menu";
extern const char op1_text[] PROGMEM="Option 1";
extern const char op2_text[] PROGMEM="Option 2";
extern const char tog12_text[] PROGMEM="toggle 1&2";
extern const char opn_text[] PROGMEM="Option ...";
extern const char exit_txt[] PROGMEM="<Exit";
extern const char max_temp_label[] PROGMEM="Max.";
extern const char max_temp_unit[] PROGMEM="C";
// extern const char yr_txt[] PROGMEM="Year";
// extern const char vcc_txt[] PROGMEM="VCC";
// extern const char volts_txt[] PROGMEM="V";

//a test variable, this is menu independent, however menu will have a reference to it and change it
int max_temp=50;

//static menu structure
Item<
  StaticMenu<
    Item<Mutable::Part,FlashText<decltype(mainMenu_title),&mainMenu_title>::Part>,
    StaticData<
      Item<EnDis<>::Part,Mutable::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
      Item<EnDis<false>::Part,Mutable::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
      Item<Action<tog12>::Part,Mutable::Part,FlashText<decltype(tog12_text),&tog12_text>::Part>,
      Item< //compose a field with a label, an edit cursor and an unit
        Mutable::Part, //track changes
        AsName<FlashText<decltype(max_temp_label),&max_temp_label>::Part>::Part,//(As) name format apply only to inner content
        WrapMode<>::Part,//(Wrap) mode format, starts here and gores to end of remaining content
        StaticNumField<int,max_temp,10,99,10,1>::Part,//the numeric field
        AsUnit<FlashText<decltype(max_temp_unit),&max_temp_unit>::Part>::Part//name format apply only to inner content
      >,
      Item<Mutable::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
      Item<
        StaticMenu<
          Item<Mutable::Part,FlashText<decltype(subMenu_title),&subMenu_title>::Part>,
          StaticData<
            Item<EnDis<>::Part,Mutable::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
            Item<Mutable::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
            Item<Mutable::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<Mutable::Part,FlashText<decltype(exit_txt),&exit_txt>::Part>
          >
        >::Part
      >
    >
  >::Part
> mainMenu;//create menu object

//menu input --------------------------------------
SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

// using SerialO=StaticMenuOut<
//   FullPrinter::Part,//print title and items
//   TitleWrapFmt<>::Part,//put [] around menu title
//   TextFmt::Part,//apply text formating
//   SerialOut<decltype(Serial),Serial>::Part//raw output device
// >;

// using DebugMonitor=StaticMenuOut<
//   ANSIMonitor<decltype(Serial),Serial>::Part//raw output device
// >;

//monitoring an output device
using Out=StaticMenuOut<
  ANSIMonitor<
    decltype(Serial),
    Serial,
    StaticMenuOut<
      FullPrinter::Part,//print title and items
      TitleWrapFmt<>::Part,//put [] around menu title
      TextCursorFmt,//draw text cursor
      TextEditModeFmt,//draw edit mode text cursor
      TextItemFmt,//item on a new line
      PartialDraw::Part,//this device can position the cursor and do a partial draw
      PanelTarget::Part,//track target (menu) change
      Viewport::Part,
      RangePanel<>::Part,//control vertical scrolling
      StaticPanel<0,0,16,2>::Part,//define output geometry
      I2CLCDOut<lcd>::Part,//raw output device
      TextMeasure<>::Part//default monometric text measure
    >
  >::Part
>;

Out out;

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
  nav.print(out);//initial menu draw
}

// long nextMenuFrame=MENURATE;
void loop(){
  if (nav.doInput(in)) nav.print(out);
  delay(100);
}
