/*
simple std console
*/

#include "linuxkb.h"

//menu system include
#include <menu.h>
//menu output devices
#include <menu/IO/consoleOut.h>
//extra menu ciomponents
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>
//output format
#include <menu/fmt/textFmt.h>
#include <menu/fmt/titleWrap.h>

/////////////////////////////////////////////////////////
const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* year_text="Year";
const char* mainMenu_title="Main menu";
const char* subMenu_title="Sub-menu";

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

bool hey() {
  cout<<"Hey!"<<endl;
  return false;
}

bool grrr() {
  cout<<"This should not be called as the option is disabled"<<endl;
  return false;
}

template<const char** text,typename O=Empty<>>
using Op=EnDis<StaticText<text,O>>;

int year=2019;

using SubMenu=Prompt<
  StaticText<
    &subMenu_title,
    StaticMenu<
      Op<&op2_text>,
      Op<&op2_text>
    >
  >
>;

using MainMenu=StaticText<
  &mainMenu_title,
  StaticMenu<
    Action<Op<&op1_text>,hey>,
    Action<Op<&op2_text>,grrr>,
    Op<
      &year_text,
      AsMode<
        AsValue<
          NavHandler<
            NumField<int,year,1900,2100,10,1>
          >
        >
      >
    >,
    SubMenu,
    StaticText<&op3_text>
  >
>;

MainMenu mainMenu;

StaticNav<
  MainMenu,
  ItemNav<NavPos<>>
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
  nav.enable(1,false);//disable second option
  nav.printMenu(out);//print initial menu

  //navigate the menu
  do {
    if (kbhit()) {
      int k=getch();
      if (k==27) {
        if (kbhit()) k=getch();
        else break;
      } else if (keys(k))
        nav.printMenu(out);
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
