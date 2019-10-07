#include <menu.h>
#include <menu/IO/consoleOut.h>
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
#include <menu/comp/endis.h>
#include "linuxkb.h"

using Out=MenuOut<
  FullPrinter<
    TextFmt<
      TitleWrapFmt<
        RangePanel<
          StaticPanel<0,0,20,4,Console>
        >
      >
    >
  >
>;

Out out;

// const char* text1_txt="standalone text";
// StaticText<&text1_txt> text1;

const char* op1_txt="Option 1";
const char* op2_txt="Option 2";
const char* op3_txt="...";
const char* main_txt="Main menu";
const char* sub_txt="Sub-menu";
using Op1=StaticText<&op1_txt>;
using Op2=StaticText<&op2_txt>;
using Op3=StaticText<&op3_txt>;

bool test() {_trace(MDO<<"this is a test action");return false;}

using MainMenu=StaticMenu<
  StaticText<&main_txt>,
  StaticData<
    Action<Op1,test>,
    Op2,
    Op3,
    Op3,
    Op3,
    StaticMenu<
      StaticText<&sub_txt>,
      StaticData<Op1,Op2,Op3>
    >,
    Op3
  >
>;

MainMenu mainMenu;

StaticRoot<
  StaticNavTree<MainMenu,2,NavPos<>>
> nav(mainMenu);

bool running=true;

//----------------------------------------------------------------
//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case 91:return false;
    case 66:case '+': return nav.up();
    case 65:case '-': return nav.down();
    case 13:case 67:case '*': return nav.enter();
    case 27:case 68:case '/': return nav.esc();
    case 3:running=false;break;
    default: _trace(MDO<<"key:"<<key<<"["<<(key>32?(char)key:'?')<<"]"<<endl);
  }
  return false;
}

int main() {
  set_conio_terminal_mode();
  //static print
  // text1.print<Console>();
  Console::raw("AM5 Tests ----------------------");
  Console::nl();
  nav.path[0]=5;
  nav.level=1;
  // MDO<<"size:"<<nav.size()<<endl;
  // nav.up();
  // nav.up();
  // nav.esc();
  // keys('+');
  // keys('+');

  //menu------------------------
  nav.print<Out>();
  do {
    if (kbhit()) {
      int k=getch();
      if (k==27) if (kbhit()) k=getch();
      if (keys(k)) nav.print<Out>();
    }
  } while(running);
  Console::nl();
  return 0;
}
