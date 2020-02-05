#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/IO/consoleOut.h>

//sketch control and actions ------------------------
bool running=true;//exit program when false

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

bool exit() {
  //just signal program exit
  _trace(MDO<<"Exit!"<<endl);
  return true;
}

//menu texts -------------------------
const char* subText="Sub-menu";
const char* sub1_text="Sub 1";
const char* sub2_text="Sub 2";
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
    StaticText<&mainText>::Part<>,
    StaticData<
      Item<StaticText<&op1_text>::Part>,
      Item<StaticText<&op2_text>::Part>,
      Item<StaticText<&opn_text>::Part>,
      Item<StaticText<&opn_text>::Part>,
      Item<
        StaticMenu<
          StaticText<&subText>::Part<>,
          StaticData<
            Item<StaticText<&sub1_text>::Part>,
            Item<StaticText<&sub2_text>::Part>,
            Item<StaticText<&subn_text>::Part>,
            Item<StaticText<&subn_text>::Part>,
            Item<
              StaticMenu<
                StaticText<&subText>::Part<>,
                StaticData<
                  Item<StaticText<&sub1_text>::Part>,
                  Item<StaticText<&sub2_text>::Part>,
                  Item<StaticText<&subn_text>::Part>,
                  Item<StaticText<&subn_text>::Part>,
                  Item<Action<exit>::Part,StaticText<&exit_text>::Part>
                >
              >::Part
            >,
            Item<Action<exit>::Part,StaticText<&exit_text>::Part>
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
  FullPrinter,
  TitleWrapFmt<>::Part,
  TextFmt,
  RangePanel<>::Part,
  Console,
  TextMeasure<>::Part
> out;

StaticNavRoot<Nav<MainMenu,5>::Part> nav(mainMenu);

//menu input --------------------------------------
LinuxKeyIn in;

int main() {
  MDO<<"AM5 tests ================================="<<endl;
  // MDO<<"root size:"<<mainMenu.size()<<endl;
  // MDO<<"root self size:"<<mainMenu.size(self)<<endl;
  // MDO<<"ref:"<<Path<1>().ref()<<endl;

  // MDO<<"root /4 size:"<<mainMenu.size(Path<4>().ref())<<endl;
  // MDO<<"root /4/1 size:"<<mainMenu.size(Path<4,1>().ref())<<endl;

  // MDO<<"root /4 canNav:"<<mainMenu.canNav(Path<4>().ref())<<endl;
  // MDO<<"root /4/4 canNav:"<<mainMenu.canNav(Path<4,4>().ref())<<endl;
  // MDO<<"root /4/5 canNav:"<<mainMenu.canNav(Path<4,5>().ref())<<endl;

  // MDO<<"root /4 isMenu:"<<mainMenu.isMenu(Path<4>().ref())<<endl;
  // MDO<<"root /4/4 isMenu:"<<mainMenu.isMenu(Path<4,4>().ref())<<endl;
  // MDO<<"root /4/5 isMenu:"<<mainMenu.isMenu(Path<4,5>().ref())<<endl;

  MDO<<"root /4 activate:"<<mainMenu.activate(Path<4>().ref())<<endl;
  MDO<<"root /4/4 activate:"<<mainMenu.activate(Path<4,4>().ref())<<endl;
  MDO<<"root /4/5 activate:"<<mainMenu.activate(Path<4,5>().ref())<<endl;
  return 0;
}
