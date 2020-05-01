/***
example for arduino (generic)
using serial for menu IO
tested on nano
*/

//main include for ArduinoMenu
#include <staticMenu.h>
//input/output drivers --------------------------------------
// #include <menu/IO/Arduino/serialIn.h>
// #include <menu/IO/Arduino/serialOut.h>
#include <menu/IO/Arduino/serialIO.h>//include both serial in and out
//format specifyers -----------------------------------------
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
// #include <menu/comp/numField.h>//numeric fields with name, value and unit
#include <menu/comp/Arduino/flashText.h>
using namespace Menu;

// some user code for example --------------------------------------------------------------
int year=1967;
int vcc=3;

//------------------------------
//menu action handlers
bool tog12();

bool op1_action() {
  Serial.println("Option 1 action called!");
  return true;//false would close the menu
}

bool op2_action() {
  Serial.println("Option 2 action called!");
  return true;
}

bool sub_action() {
  Serial.println("Sub-menu action called!");
  return true;
}

enum MyIds:Idx {
  id_mainMenu,
  id1,
  id2,
  id3
};

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
Item<
  IdTag<id_mainMenu>::Part,
  StaticMenu<
    FlashText<decltype(mainMenu_title),&mainMenu_title>::Part<>,
    StaticData<
      Item<IdTag<id1>::Part,Action<op1_action>::Part,EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
      Item<IdTag<id2>::Part,Action<op2_action>::Part,EnDis<false>::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
      Item<IdTag<id3>::Part,Action<tog12>::Part,FlashText<decltype(tog12_text),&tog12_text>::Part>,
      Item<FlashText<decltype(opn_text),&opn_text>::Part>,
      Item<
        StaticMenu<
          FlashText<decltype(subMenu_title),&subMenu_title>::Part<>,
          StaticData<
            Item<Action<sub_action>::Part,EnDis<>::Part,FlashText<decltype(op1_text),&op1_text>::Part>,
            Item<Action<sub_action>::Part,FlashText<decltype(op2_text),&op2_text>::Part>,
            Item<Action<sub_action>::Part,FlashText<decltype(opn_text),&opn_text>::Part>,
            Item<FlashText<decltype(exit_txt),&exit_txt>::Part>
          >
        >::Part
      >
    >
  >::Part
> mainMenu;//create menu object

//menu input --------------------------------------
StaticMenuIn<
  SerialIn<decltype(Serial),Serial>::Part,
  AMNavKeys<>::Part,
  Ids::Part
> in;//create input object (here serial)

//menu output (Serial)
using Out=StaticMenuOut<
  FullPrinter::Part,//print title and items
  TitleWrapFmt<>::Part,//put [] around menu title
  TextFmt::Part,//apply text formating
  SerialOut<decltype(Serial),Serial>::Part//raw output device
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
StaticNavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

//menu action handlers implementation
bool tog12() {
  Serial.println("Toggle enable/diable of options 1 and 2");
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("ArduinoMenu 5");
  delay(500);
  nav.print(out);
}

void loop() {
  if (nav.doInput(in)) nav.print(out);
}
