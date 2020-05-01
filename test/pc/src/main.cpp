//this is part of development files and might not work
//its just for testing and experimenting
#include <iostream>
using namespace std;

#include <staticMenu.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>
using namespace Menu;

const char* mainText="Main menu";
const char* text="Option 1";
const char* quit_text="Quit!";
const char* subText="Sub-menu";
const char* sub1_text="Sub 1";

//sketch control and actions ------------------------
bool running=true;//exit program when false

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return false;
}

Item<
  StaticMenu<
    Item<StaticText<&mainText>::Part,Mutable::Part>,
    StaticData<
      Item<StaticText<&text>::Part,Mutable::Part>,
      Item<
        StaticMenu<
          Item<StaticText<&subText>::Part,Mutable::Part>,
          StaticData<
            Item<StaticText<&sub1_text>::Part,Mutable::Part>
          >
        >::Part
      >,
      Item<Action<quit>::Part,StaticText<&quit_text>::Part,Mutable::Part>
    >
  >::Part
> mainMenu;

StaticMenuOut<
  FullPrinter::Part,//print all parts, title, index, text cursor
  TitleWrapFmt<>::Part,//wrap title in []
  TextFmt::Part,//format the text parts, use `>` as text cursor`
  Console::Part,//the raw output device to use
  TextMeasure<>::Part//default monometric text measure
> out;

StaticNavRoot<Nav<decltype(mainMenu),2>::Part> nav(mainMenu);

//menu input --------------------------------------
StaticMenuIn<
  LinuxKeyIn::Part,
  PCArrows::Part
> in;

int main() {
  // nav.up();
  // nav.enter();
  // nav.enter();
  nav.print(out);
  while(running) if (nav.doInput(in)) nav.print(out);
  return 0;
}
