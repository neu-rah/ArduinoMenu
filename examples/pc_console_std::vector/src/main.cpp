#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>
#include <menu/comp/endis.h>//enable/disable
using namespace Menu;

bool running=true;

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

bool tog12();//implemented later because we need to access mainMenu

bool action1() {
  cout<<"Action1 called!"<<endl;
  return true;
}

bool action2() {
  cout<<"Action2 called!"<<endl;
  return true;
}

bool sub1Action() {
  cout<<"Sub 1 action called!"<<endl;
  return true;
}

//menu data/texts ----------------------------
const char* mainText="Main menu";
Item<Mutable,StaticText<&mainText>> title;

Prompt<Action<action1>,EnDis<>,Text,Mutable> op1("Option 1");
Prompt<Action<action2>,EnDis<false>,Text,Mutable> op2("Option 2");
Prompt<Action<tog12>,Mutable,Text> tog12Op("Toggle 1&2");
const char*opn_text="Option...";//sharing same text
Prompt<Mutable,Text> op3(opn_text);
Prompt<Mutable,Text> op4(opn_text);
Prompt<Action<quit>,Mutable,Text> quitOp("<Quit.");

Prompt<Mutable,Text> sub_title("Sub-menu");
Prompt<Action<action1>,Mutable,Text> sub1("Sub 1");
Prompt<Mutable,Text> sub2("Sub 2");
Prompt<Mutable,Text> subn("Sub...");
Prompt<Mutable,Text> exitOp("<Exit");

//menu structure -------------------------
Prompt<EnDis<true>,StdVectorMenu<decltype(sub_title),sub_title>> subMenu {
  &sub1,
  &sub2,
  &subn,
  &exitOp
};

Prompt<StdVectorMenu<decltype(title),title>> mainMenu {
  &op1,
  &op2,
  &tog12Op,
  &op3,
  &op4,
  &subMenu,
  &quitOp
};

//menu navigation control -------------------------
NavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

//menu output --------------------------------------
MenuOut<
  FullPrinter::Part,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt::Part,//format the text parts, use `>` as text cursor`
  // PartialDraw,//just for testing, because console is not ob this cathegory
  // PanelTarget,//detect target (menu) changes
  // RangePanel<>::Part,//control vertical scrolling
  // StaticPanel<0,0,20,30>::Part,//define output geometry
  Console::Part,//the raw output device to use
  TextMeasure<>::Part//default monometric text measure
> out;

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
  while(running) if (nav.doInput(in)) nav.print(out);
  return 0;
}
