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

bool sub1_action() {
  cout<<"Sub 1 action called!"<<endl;
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
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

//menu static structure ---------------------------
using MainMenu=Item<
  StaticMenu<
    Item<StaticText<&mainText>::Part>,
    StaticData<
      Item<Action<op1_action>::Part,EnDis<>::Part,StaticText<&op1_text>::Part>,
      Item<Action<op2_action>::Part,EnDis<false>::Part,StaticText<&op2_text>::Part>,
      Item<Action<tog12>::Part,StaticText<&tog12_text>::Part>,
      Item<StaticText<&opn_text>::Part>,
      Item<StaticText<&opn_text>::Part>,
      Item<
        StaticMenu<
          Item<StaticText<&subText>::Part>,
          StaticData<
            Item<Action<sub1_action>::Part,StaticText<&sub1_text>::Part>,
            Item<StaticText<&sub2_text>::Part>,
            Item<StaticText<&subn_text>::Part>,
            Item<StaticText<&exit_text>::Part>
          >
        >::Part
      >,
      Item<StaticText<&opn_text>::Part>,
      Item<Action<quit>::Part,StaticText<&quit_text>::Part>
    >
  >::Part
>;

MainMenu mainMenu;

//menu output ---------------------------------------
StaticMenuOut<
  FullPrinter,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt,//format the text parts, use `>` as text cursor`
// #ifdef MENU_DEBUG
//   PartialDraw,//just for testing, because console is not of this cathegory
//   PanelTarget,//detect target (menu) changes
//   RangePanel<>::Part,//control vertical scrolling
//   StaticPanel<0,0,20,6>::Part,//define output geometry
// #endif
  Console,//the raw output device to use
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
