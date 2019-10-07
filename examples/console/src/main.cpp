#include <menu.h>
#include <menu/IO/consoleOut.h>
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
#include <menu/comp/endis.h>
#include "linuxkb.h"

bool running=true;

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

// Out out;

// const char* text1_txt="standalone text";
// StaticText<&text1_txt> text1;

const char* op1_txt="Option 1";
const char* op2_txt="Option 2";
const char* op3_txt="...";
const char* exit_txt="<Quit";
const char* main_txt="Main menu";
const char* sub_txt="Sub-menu";
using Op1=StaticText<&op1_txt>;
using Op2=StaticText<&op2_txt>;
using Op3=StaticText<&op3_txt>;

bool test() {
  _trace(MDO<<"this is a test action"<<endl);
  _trace(MDO<<"no context provided but we can have it"<<endl);
  _trace(MDO<<"by accessing the navigation root object"<<endl);
  _trace(MDO<<"assuming only one assigned to this action"<<endl);
  return true;
}
bool quit() {
  //just signal program exit
  return running=false;
}

template<typename I>
struct MyAction:I {
  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav) {
    cout<<"custom action with context"<<endl;
    cout<<"we received target and nav objects"<<endl;
    cout<<"Nav at:"<<nav.at<<endl;
    cout<<"level:"<<nav.level<<endl;
    Ref ref=nav;
    cout<<"path:";
    for(int n=0;n<ref.len;n++)
      cout<<"/"<<nav.path[n];
    cout<<"/"<<nav.at;
  }
};

using MainMenu=StaticMenu<
  StaticText<&main_txt>,
  StaticData<
    Action<Op1,test>,
    MyAction<Op2>,
    Op3,
    Op3,
    Op3,
    Debug<StaticMenu<
      StaticText<&sub_txt>,
      StaticData<
        MyAction<Op1>,
        Action<Op2,test>,
        Op3
      >
    >>,
    Op3,
    Action<StaticText<&exit_txt>,quit>
  >
>;

MainMenu mainMenu;

StaticRoot<
  StaticNavTree<MainMenu,2,NavPos<>>
> nav(mainMenu);

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
  Console::raw("AM5 Tests ----------------------");
  Console::nl();
  // nav.path[0]=5;
  // nav.level=1;
  nav.at=7;
  nav.up();

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
