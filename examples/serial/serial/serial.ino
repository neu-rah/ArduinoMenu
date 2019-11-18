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
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>//enable/disable
#include <menu/comp/numField.h>//numeric fields with name, value and unit

// some user code for example --------------------------------------------------------------
int year=1967;
int vcc=3;

//------------------------------
//menu action handlers
bool tog12();

// define menu structure ---------------------------------------------------------------

//texts for menu
const char* mainMenu_title="Main menu";
const char* subMenu_title="Sub-menu";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* tog12_text="toggle 1&2";
const char* opn_text="Option ...";
const char* exit_txt="<Exit";
const char* quit_txt="<Quit";
const char* yr_txt="Year";
const char* vcc_txt="VCC";
const char* volts_txt="V";

//static menu structure
StaticMenu<
  Item<StaticText<&mainMenu_title>>,
  StaticData<
    Item<EnDis<StaticText<&op1_text>>>,
    Item<EnDis<StaticText<&op2_text>>>,
    Item<Action<StaticText<&tog12_text>,tog12>>,
    Item<NumField<StaticText<&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changing limits generates new code->TODO: add a translation
    Item<NumField<StaticText<&vcc_txt>,decltype(vcc),vcc,0,100,1,0,StaticText<&volts_txt>>>,
    Item<StaticText<&opn_text>>,
    StaticMenu<
      Item<StaticText<&subMenu_title>>,
      StaticData<
        Item<EnDis<StaticText<&op1_text>>>,
        Item<StaticText<&op2_text>>,
        Item<StaticText<&opn_text>>,
        Item<StaticText<&opn_text>>,
        Item<StaticText<&opn_text>>,
        Item<Exit<StaticText<&exit_txt>>>
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
        RangePanel<//scroll content on output geometry
          StaticPanel<0,0,20,4,SerialOut<decltype(Serial),Serial>>//describe output geometry and device
        >
      >
    >
  >
>;

Out out;//create output object (Serial)

//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

//menu action handlers implementation
bool tog12() {
  // Idx fstPath[]{0};
  // Ref fst{0,fstPath};
  // Idx sndPath[]{0};
  // Ref snd{0,sndPath};
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
