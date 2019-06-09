/*
simple std console
*/

#include "linuxkb.h"

#include <menu.h>
#include <menu/fmt/textFmt.h>
#include <menu/fmt/titleWrap.h>
#include <menu/IO/consoleOut.h>

/////////////////////////////////////////////////////////
const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* mainMenu_title="Main menu";

using Op1=SelfNav<
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

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case 66:case '+': return nav.up();
    case 65:case '-': return nav.down();
    case 13:case 67:case '*': return nav.enter();
    case 27:case 68:case '/': return nav.esc();
    default: cout<<"key:"<<key<<"["<<(key>32?(char)key:'?')<<"]"<<endl;
  }
  return false;
}

//-----------------------
int main() {
  cout<<"PIO Native program"<<endl;
  set_conio_terminal_mode();
  nav.printMenu(out);

  do {
    if (kbhit()) {
      int k=getch();
      if (k==27) {
        if (kbhit()) k=getch();
        else break;
      } else if (keys(k)) nav.printMenu(out);
    }
  } while(true);
  while(kbhit()) cout<<(int)getch()<<endl;
  // op1.printMenu(out);//TODO: fic this, sigle element printing
  // nav.printMenu(out);
  // nav.up();
  // nav.down();
  // nav.up();
  // nav.printMenu(out);
  return 0;
}
