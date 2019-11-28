#include <iostream>
using namespace std;

//main include for ArduinoMenu
#include <menu.h>
//input/output drivers --------------------------------------
#include <menu/IO/consoleOut.h>
#include <menu/IO/linuxKeyIn.h>
//format specifyers -----------------------------------------
#include <menu/fmt/ANSI.h>//to draw index and text cursors (nav and edit)
#include <menu/fmt/textEditMode.h>
#include <menu/fmt/textCursor.h>
#include <menu/fmt/textItem.h>
#include <menu/fmt/titleWrap.h>
//components ------------------------------------------------
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>

bool running=true;

bool tog12();
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
const char* tog12_text="toggle 1&2";
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
    Item<EnDis<StaticText<&op2_text>,false>>,//define as disabled on startup
    Item<Action<tog12,StaticText<&tog12_text>>>,
    Item<NumField<StaticText<&yr_txt>,int,year,1900,2100,10,1>>,//this is NOT good, changing limits generates new code->TODO: add a translation
    Item<NumField<StaticText<&vcc_txt>,decltype(vcc),vcc,0,100,1,0,StaticText<&volts_txt>>>,
    Item<StaticText<&opn_text>>,
    Item<StaticText<&opn_text>>,
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
    Item<Action<quit,StaticText<&quit_txt>>>
  >
> mainMenu;

//menu input --------------------------------------
LinuxKeyIn in;
//menu output -------------------------------------

//ANSI terminal output
using Out=FullPrinter<//print title and items
  Fmt<//formating API
    TitleWrapFmt<//put [] around menu title
      PartialDraw<
        AnsiFmt<//ansi text format
          TextCursorFmt<//draw text cursor
            TextEditModeFmt<//draw edit mode text cursor
              TextItemFmt<//add newline after each item
                CanMeasure<
                  PanelTarget<//track panel drawn menu changes
                    RangePanel<//scroll content on output geometry
                      StaticPanel<50,2,20,6,Console>//describe output geometry and device
                    >
                  >
                >
              >
            >
          >
        >
      >
    >
  >
>;

Out out;

//navigation root ---------------------------------
Nav<decltype(mainMenu),mainMenu,2> nav;

bool tog12() {
  _trace(MDO<<"to12:"<<mainMenu.enabled(0)<<mainMenu.enabled(1));
  mainMenu.enable(!mainMenu.enabled(0),0);
  mainMenu.enable(!mainMenu.enabled(1),1);
  _trace(MDO<<"->"<<mainMenu.enabled(0)<<mainMenu.enabled(1));
  return true;
}

int main() {
  nav.printMenu(out);
  while(running) {
    if (nav.doInput(in)) nav.printMenu(out);
    cout.flush();
  };
  out.nl();
  return 0;
}
