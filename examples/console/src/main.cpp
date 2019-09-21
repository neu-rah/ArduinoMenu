#include <iostream>
using namespace std;
#include <menu.h>
#include <menu/fmt/textFmt.h>
#include <menu/fmt/titleWrap.h>
#include "linuxkb.h"

////////////////////////////////////////////////////////////////////////////////
// testing stuff
using Out=
MenuOut<
  TextFmt<
    TitleWrapFmt<
      FullPrinter<
        RangePanel<
          StaticPanel<0,0,20,4,Console>
        >
      >
    >
  >
>;

const char* singleOp_text="Standalone item";
using SingleOp=StaticText<&singleOp_text>;

const char* combinedContent_text="This is a combined ";
using CombinedContent=StaticText<&combinedContent_text,SingleOp>;

const char* op1_text="Option A";
const char* op2_text="Option B";
const char* opn_text="...";
const char* altTitle_text="Alt. title for aux. menu!";

const char*auxMenu_title="Aux. menu";
using AuxMenu=Item<
  StaticMenu<
    Map<//agregate multiple elements to be processed in order
      NoTitle<StaticText<&auxMenu_title>>,//use this except for menu title
      AsTitle<StaticText<&altTitle_text>>//the alternative title
    >,
    StaticMenuData<
      StaticText<&op1_text>,
      StaticText<&op2_text>,
      SingleOp
    >
  >
>;

const char* mainMenu_text="Main menu";
using MainMenu=Item<
  StaticMenu<
    StaticText<&mainMenu_text>,
    StaticMenuData<
      StaticText<&op1_text>,
      StaticText<&op2_text>,
      StaticText<&opn_text>,
      StaticText<&opn_text>,
      StaticText<&opn_text>,
      StaticText<&opn_text>,
      AuxMenu
    >
  >
>;

NavNode<
  NavPos<
    StaticFlatNavNode<
      MainMenu
    >
  >
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

int main() {
  set_conio_terminal_mode();
  Out::raw("AM5 development.===================================================");
  Out::nl();
  nav.printMenu<Out>();

  do {
    if (kbhit()) {
      int k=getch();
      if (k==27) {
        if (kbhit()) k=getch();else break;
      } else if (keys(k))nav.printMenu<Out>();
    }
  } while(true);
  while(kbhit()) cout<<(int)getch()<<endl;
}
