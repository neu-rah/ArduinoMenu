/*
simple console
*/

#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/fmt/textFmt.h>
#include <menu/fmt/titleWrap.h>

/////////////////////////////////////////////////////////
const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* mainMenu_title="Main menu";

using Op1=ItemNav<
  StaticItem<
    StaticText<&op1_text>
  >
>;

Op1 op1;
TextFmt<
  TitleWrapFmt<
    FullPrinter<
      StdOut<>
    >
  >
> out;

using MainMenu=StaticText<
  &mainMenu_title,
  StaticMenu<
    StaticText<&op1_text>,
    StaticText<&op2_text>,
    StaticText<&op3_text>
  >
>;

MainMenu mainMenu;

StaticNav<
  MainMenu,
  NavPos<>
> nav;

//-----------------------
int main() {
  cout<<"PIO Native program"<<endl;
  // op1.printMenu(out);//TODO: fic this, sigle element printing
  nav.printMenu(out);
  nav.up();
  nav.down();
  nav.up();
  nav.printMenu(out);
  return 0;
}
