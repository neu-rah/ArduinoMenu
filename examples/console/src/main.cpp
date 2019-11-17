#include <iostream>
using namespace std;

//main include for ArduinoMenu
#include <menu.h>
//input/output drivers --------------------------------------
#include <menu/IO/consoleOut.h>
#include <menu/IO/linuxKeyIn.h>
//format specifyers -----------------------------------------
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>

bool running=true;

bool quit() {
  //just signal program exit
  trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

Text<> test("Just a test");

const char* mainMenu_title="Main menu";
const char* subMenu_title="Sub-menu";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option ...";
const char* exit_txt="<Exit";
const char* quit_txt="<Quit";
const char* yr_txt="Year";
const char* vcc_txt="VCC";
const char* volts_txt="V";

int year=1967;
int vcc=3;

StaticMenu<
  Item<StaticText<&mainMenu_title>>,
  StaticData<
    Item<EnDis<StaticText<&op1_text>>>,
    Item<EnDis<StaticText<&op2_text>>>,
    Item<NumField<StaticText<&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changung limits generates new code->TODO: add a translation
    Item<NumField<StaticText<&vcc_txt>,decltype(vcc),vcc,0,100,1,0,AsUnit<StaticText<&volts_txt>>>>,
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
    >,
    Item<Action<StaticText<&quit_txt>,quit>>
  >
> mainMenu;

//menu input --------------------------------------
LinuxKeyIn in;
//menu output -------------------------------------
FullPrinter<Fmt<TitleWrapFmt<TextFmt<Console>>>> out;
//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

int main() {
  Console::raw("AM5 Tests ----------------------");
  Console::nl();
  test.print(out);//printing single field
  Console::nl();
  nav.printMenu(out);
  while(running) {
    if (nav.doInput(in)) nav.printMenu(out);
    cout.flush();
  };
  out.nl();
  return 0;
}
