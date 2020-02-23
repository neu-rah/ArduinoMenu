#include <iostream>
using namespace std;

#include <staticMenu.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>
#include <menu/comp/endis.h>//enable/disable
using namespace Menu;

//sketch control and actions ------------------------
bool running=true;//exit program when false

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}
bool tog12();//implemented later because we need to access mainMenu
 
bool op1_action() {
  cout<<"Option 1 action called!"<<endl;
  return true;//false would close the menu
}

bool op2_action() {
  cout<<"Option 2 action called!"<<endl;
  return true;
}

bool sub_action() {
  cout<<"Sub-menu action called!"<<endl;
  return true;
}

//menu texts -------------------------
const char* subText="Sub-menu";
const char* sub1_text="Sub 1";
const char* sub2_text="Sub 2";
const char* tog12_text="Toggle 1&2";
const char* subn_text="Sub...";
const char* exit_text="<Exit";

const char* mainText="Main menu";
const char* year_label_text="Year";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

int year=1967;

//menu static structure ---------------------------
using MainMenu=Item<
  StaticMenu<
    Item<StaticText<&mainText>,Mutable>,
    StaticData<
      Item<Action<op1_action>,EnDis<>,StaticText<&op1_text>,Mutable>,
      Item<Action<op2_action>,EnDis<false>,StaticText<&op2_text>,Mutable>,
      Item<Action<tog12>,StaticText<&tog12_text>,Mutable>,
      Item<
        AsName<StaticText<&year_label_text>>,
        AsMode<>,
        StaticNumField<int,year,1900,3000,10,1>,
        Mutable
      >,
      Item<StaticText<&opn_text>,Mutable>,
      Item<
        StaticMenu< 
          Item<StaticText<&subText>,Mutable>,
          StaticData<
            Item<Action<sub_action>,StaticText<&sub1_text>,Mutable>,
            Item<Action<sub_action>,StaticText<&sub2_text>,Mutable>,
            Item<StaticText<&exit_text>,Mutable>
          >
        >
      >,
      Item<Action<quit>,StaticText<&quit_text>,Mutable>
    >
  >
>;

MainMenu mainMenu;

//menu output ---------------------------------------
StaticMenuOut<
  FullPrinter::Part,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt::Part,//format the text parts, use `>` as text cursor`
// #ifdef MENU_DEBUG
//   PartialDraw::Part,//just for testing, because console is not of this cathegory
//   PanelTarget::Part,//detect target (menu) changes
//   RangePanel<>::Part,//control vertical scrolling
//   StaticPanel<0,0,20,4>::Part,//define output geometry
// #endif
  Console::Part,//the raw output device to use
  TextMeasure<>::Part//default monometric text measure
> out;

StaticNavRoot<Nav<MainMenu,3>::Part> nav(mainMenu);

//menu input --------------------------------------
LinuxKeyIn in;

bool tog12() {
  _trace(MDO<<"Toggle Enable/Disable of options 1 and 2"<<endl);
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

int main() {
  nav.print(out);
  while(running) {
    if (nav.doInput(in)) {
      nav.print(out);
      out.nl();
    }
    cout.flush();
  };
  return 0;
}
