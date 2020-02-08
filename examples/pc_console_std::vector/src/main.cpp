#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>

bool running=true;

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

bool exit() {
  //just signal program exit
  _trace(MDO<<"Exit!"<<endl);
  return false;
}

//menu data/texts ----------------------------
const char* mainText="Main menu";
Item<StaticText<&mainText>::Part> title;

Prompt<Text> op1("Option 1");
Prompt<Text> op2("Option 2");
Prompt<Text> opn("Option...");
Prompt<Action<quit>::Part,Text> quitOp("<Quit.");

Prompt<Text> sub_title("Sub-menu");
Prompt<Text> sub1("Sub 1");
Prompt<Text> sub2("Sub 2");
Prompt<Text> subn("Sub...");
Prompt<Action<exit>::Part,Text> exitOp("<Exit");

//menu structure -------------------------
Prompt<StdVectorMenu<decltype(sub_title),sub_title>::Part> subMenu {
  &sub1,
  &sub2,
  &subn,
  &exitOp
};

Prompt<StdVectorMenu<decltype(title),title>::Part> mainMenu {
  &op1,
  &op2,
  &opn,
  &opn,
  &subMenu,
  &quitOp
};

//menu navigation control -------------------------
NavRoot<Nav<decltype(mainMenu),3>::Part> nav(mainMenu);

//menu output --------------------------------------
MenuOut<
  FullPrinter,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt,//format the text parts, use `>` as text cursor`
  RangePanel<>::Part,//control vertical scrolling
  StaticPanel<0,0,20,4>::Part,//define output geometry
  Console,//the raw output device to use
  TextMeasure<>::Part//default monometric text measure
> out;

//menu input --------------------------------------
LinuxKeyIn in;

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
