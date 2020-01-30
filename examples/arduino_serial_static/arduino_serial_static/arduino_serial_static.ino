/***
example for arduino (generic)
using serial for menu IO
tested on nano
*/

//main include for ArduinoMenu
#include <menu.h>
//input/output drivers --------------------------------------
// #include <menu/IO/Arduino/serialIn.h>
#include <menu/IO/Arduino/serialOut.h>
// #include <menu/IO/Arduino/serialIO.h>//include both serial in and out
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
// #include <menu/comp/numField.h>//numeric fields with name, value and unit
#include <menu/comp/Arduino/flashText.h>

// some user code for example --------------------------------------------------------------
int year=1967;
int vcc=3;

//------------------------------
//menu action handlers
bool tog12();

// define menu structure ---------------------------------------------------------------

//texts for menu
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
StaticMenu<
  Item<FlashText<decltype(mainMenu_title),&mainMenu_title>::As>,
  StaticData<
    Item<EnDis<FlashText<decltype(op1_text),&op1_text>>::As>,
    Item<EnDis<FlashText<decltype(op2_text),&op2_text>>::As>,
    Item<Action<tog12,FlashText<decltype(tog12_text),&tog12_text>>::As>,
    // Item<NumField<FlashText<decltype(yr_txt),&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changing limits generates new code->TODO: add a translation
    // Item<NumField<FlashText<decltype(vcc_txt),&vcc_txt>,decltype(vcc),vcc,0,100,1,0,FlashText<decltype(volts_txt),&volts_txt>>>,
    Item<FlashText<decltype(opn_text),&opn_text>::As>,
    StaticMenu<
      Item<FlashText<decltype(subMenu_title),&subMenu_title>::As>,
      StaticData<
        Item<EnDis<FlashText<decltype(op1_text),&op1_text>>::As>,
        Item<FlashText<decltype(op2_text),&op2_text>::As>,
        Item<FlashText<decltype(opn_text),&opn_text>::As>,
        Item<FlashText<decltype(opn_text),&opn_text>::As>,
        Item<FlashText<decltype(opn_text),&opn_text>::As>,
        Item<Exit<FlashText<decltype(exit_txt),&exit_txt>>::As>
      >
    >
  >
> mainMenu;//create menu object

//menu input --------------------------------------
// SerialIn<decltype(Serial),Serial> in;//create input object (here serial)

//menu output (Serial)
using Out=StaticMenuOut<
  FullPrinter,//print title and items
  TitleWrapFmt<>::As,//put [] around menu title
  TextFmt,//apply text formating
  SerialOut<decltype(Serial),Serial>::As//describe output geometry and device
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
StaticNavRoot<Nav<decltype(mainMenu),1>::As> nav(mainMenu);

//menu action handlers implementation
bool tog12() {
  // mainMenu.enable(!mainMenu.enabled(0),0);
  // mainMenu.enable(!mainMenu.enabled(1),1);
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("ArduinoMenu 5");
  delay(500);
  // test.print(out);//printing single field
  nav.print(out);
  // mainMenu.enable(false,1);//disable second option
}

void loop() {
  // if (nav.doInput(in)) nav.printMenu(out);
}
