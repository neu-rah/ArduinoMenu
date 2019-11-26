/***
example for arduino (generic)
using serial for menu IO
tested on nano
*/

//main include for ArduinoMenu
#include <menu.h>
//input/output drivers --------------------------------------
// #include <menu/IO/Arduino/serialIn.h>
// #include <menu/IO/Arduino/serialOut.h>
#include <menu/IO/Arduino/serialIO.h>//include both serial in and out
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
#include <menu/comp/numField.h>//numeric fields with name, value and unit
#include <menu/comp/Arduino/flashText.h>

// some user code for example --------------------------------------------------------------
int year=1967;
int vcc=3;

//------------------------------
//menu action handlers
bool tog12();

// define menu structure ---------------------------------------------------------------

//texts for menu
const char mainMenu_title[] PROGMEM="Main menu";
const char subMenu_title[] PROGMEM="Sub-menu";
const char op1_text[] PROGMEM="Option 1";
const char op2_text[] PROGMEM="Option 2";
const char tog12_text[] PROGMEM="toggle 1&2";
const char opn_text[] PROGMEM="Option ...";
const char exit_txt[] PROGMEM="<Exit";
const char yr_txt[] PROGMEM="Year";
const char vcc_txt[] PROGMEM="VCC";
const char volts_txt[] PROGMEM="V";

//static menu structure
StaticMenu<
  Item<FlashText<decltype(mainMenu_title),&mainMenu_title>>,
  StaticData<
    Item<EnDis<FlashText<decltype(op1_text),&op1_text>>>,
    Item<EnDis<FlashText<decltype(op2_text),&op2_text>>>,
    Item<Action<FlashText<decltype(tog12_text),&tog12_text>,tog12>>,
    Item<NumField<FlashText<decltype(yr_txt),&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changing limits generates new code->TODO: add a translation
    Item<NumField<FlashText<decltype(vcc_txt),&vcc_txt>,decltype(vcc),vcc,0,100,1,0,FlashText<decltype(volts_txt),&volts_txt>>>,
    Item<FlashText<decltype(opn_text),&opn_text>>,
    StaticMenu<
      Item<FlashText<decltype(subMenu_title),&subMenu_title>>,
      StaticData<
        Item<EnDis<FlashText<decltype(op1_text),&op1_text>>>,
        Item<FlashText<decltype(op2_text),&op2_text>>,
        Item<FlashText<decltype(opn_text),&opn_text>>,
        Item<FlashText<decltype(opn_text),&opn_text>>,
        Item<FlashText<decltype(opn_text),&opn_text>>,
        Item<Exit<FlashText<decltype(exit_txt),&exit_txt>>>
      >
    >
  >
> mainMenu;//create menu object

//menu input --------------------------------------
SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

//menu output (Serial)
using Out=FullPrinter<//print title and items
  Fmt<//formating API
    TitleWrapFmt<//put [] around menu title
      TextFmt<//apply text formating
        SerialOut<decltype(Serial),Serial>//describe output geometry and device
      >
    >
  >
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

//menu action handlers implementation
bool tog12() {
  mainMenu.enable(!mainMenu.enabled(0),0);
  mainMenu.enable(!mainMenu.enabled(1),1);
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("ArduinoMenu 5");
  delay(500);
  // test.print(out);//printing single field
  nav.printMenu(out);
  mainMenu.enable(false,1);//disable second option
}

void loop() {
  if (nav.doInput(in)) nav.printMenu(out);
}
