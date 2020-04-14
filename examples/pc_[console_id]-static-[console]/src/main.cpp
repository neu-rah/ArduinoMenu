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
// bool tog12();//implemented later because we need to access mainMenu

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
const char* max_temp_label="Max.";
const char* max_temp_unit="ºC";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

int max_temp=80;

enum MyIds:Idx {
  id_mainMenu,
  id1,
  id2,
  id3
};

//menu static structure ---------------------------
using MainMenu=Item<
  IdTag<id_mainMenu>::Part,
  StaticMenu<
    Item<StaticText<&mainText>::Part,Mutable::Part>,
    StaticData<
      Item<IdTag<id1>::Part,Action<op1_action>::Part,EnDis<>::Part,StaticText<&op1_text>::Part,Mutable::Part>,
      // Item<IdTag<id2>::Part,Action<op2_action>::Part,EnDis<false>::Part,StaticText<&op2_text>::Part,Mutable::Part>,
      // Item<IdTag<id3>::Part,Action<tog12>::Part,StaticText<&tog12_text>::Part,Mutable::Part>,
      // Item< //compose a field with a label, an edit cursor and an unit
      //   AsName<StaticText<&max_temp_label>::Part>::Part,//(As) name format apply only to inner content
      //   WrapMode<>::Part,//(Wrap) mode format, starts here and gores to end of remaining content
      //   StaticNumField<int,max_temp,10,99,10,1>::Part,//the numeric field
      //   AsUnit<StaticText<&max_temp_unit>::Part>::Part,//name format apply only to inner content
      //   Mutable::Part //track changes
      // >,
      // Item<StaticText<&opn_text>::Part,Mutable::Part>,
      // Item<
      //   StaticMenu<
      //     Item<StaticText<&subText>::Part,Mutable::Part>,
      //     StaticData<
      //       Item<Action<sub_action>::Part,StaticText<&sub1_text>::Part,Mutable::Part>,
      //       Item<Action<sub_action>::Part,StaticText<&sub2_text>::Part,Mutable::Part>,
      //       Item<StaticText<&exit_text>::Part,Mutable::Part>
      //     >
      //   >::Part
      // >,
      Item<Action<quit>::Part,StaticText<&quit_text>::Part,Mutable::Part>
    >
  >::Part
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
// StaticNavRoot<IdNav<decltype(mainMenu)>::Part> nav(mainMenu);

//menu input --------------------------------------
LinuxKeyIn<PCArrows::Part> in;

// bool tog12() {
//   _trace(MDO<<"Toggle Enable/Disable of options 1 and 2"<<endl);
//   mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
//   mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
//   return true;
// }

int main() {
  auto dis1=API::Enable<false>();
  mainMenu.body.walkId<decltype(dis1),id1>(dis1);
  nav.print(out);
  while(running) {
    if (nav.doInput(in)) {
      nav.print(out);
      // nav.template print<decltype(out),id_mainMenu>(out);
      out.nl();
    }
    cout.flush();
  };
  return 0;
}
