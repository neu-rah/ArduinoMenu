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
Item<StaticText<&mainText>::Part,Mutable::Part> title;

Prompt<Action<action1>::Part,EnDis<>::Part,Text::Part,Mutable::Part> op1("Option 1");
Prompt<Action<action2>::Part,EnDis<false>::Part,Text::Part,Mutable::Part> op2("Option 2");
Prompt<Action<tog12>::Part,Mutable::Part,Text::Part> tog12Op("Toggle 1&2");
const char*opn_text="Option...";//sharing same text
Prompt<Mutable::Part,Text::Part> op3(opn_text);
Prompt<Mutable::Part,Text::Part> op4(opn_text);
Prompt<Action<quit>::Part,Mutable::Part,Text::Part> quitOp("<Quit.");

Prompt<Mutable::Part,Text::Part> sub_title("Sub-menu");
Prompt<Action<action1>::Part,Mutable::Part,Text::Part> sub1("Sub 1");
Prompt<Mutable::Part,Text::Part> sub2("Sub 2");
Prompt<Mutable::Part,Text::Part> subn("Sub...");
Prompt<Mutable::Part,Text::Part> exitOp("<Exit");

int max_temp=80;

// Prompt< //compose a field with a label, an edit cursor and an unit
//   AsName<Text::Part>::Part,//(As) name format apply only to inner content
//   WrapMode<>::Part,//(Wrap) mode format, starts here and gores to end of remaining content
//   StaticNumFieldCore<int,max_temp,10,99,10,1>::Part,//the numeric field
//   AsUnit<Text::Part>::Part,//name format apply only to inner content
//   Mutable::Part //track changes
// > maxTemp("Max.","ºC");

Prompt<
  StaticNumField<
    Text::Part,//title
    int,max_temp,0,100,10,1,//parameters
    Text::Part//unit (optional)
  >::template Part
> maxTemp("Max.","ºC");

// menu structure -------------------------
Prompt<EnDis<true>::Part,StdVectorMenu<decltype(sub_title),sub_title>::Part> subMenu {
  &sub1,
  &sub2,
  &subn,
  &exitOp
};

Prompt<StdVectorMenu<decltype(title),title>::Part> mainMenu {
  &op1,
  &op2,
  &tog12Op,
  &maxTemp,
  &op3,
  &op4,
  &subMenu,
  &quitOp
};

//menu navigation control -------------------------
using MyNav=NavRoot<Nav<decltype(mainMenu),3>::Part>;
MyNav nav(mainMenu);

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
StaticMenuIn<
  LinuxKeyIn::Part,
  PCArrows::Part
> in;

bool tog12() {
  _trace(MDO<<"Toggle Enable/Disable of options 1 and 2"<<endl);
  mainMenu.enable(!mainMenu.enabled(Path<0>().ref()),Path<0>().ref());
  mainMenu.enable(!mainMenu.enabled(Path<1>().ref()),Path<1>().ref());
  return true;
}

int main() {
  nav.print(out);
  while(running) if (nav.doInput(in)) {
    cout<<"@"<<nav.operator PathRef()<<endl;
    nav.print(out);
  }
  return 0;
}
