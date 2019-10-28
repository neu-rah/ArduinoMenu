#include <signal.h>
#include <menu.h>
#include <menu/IO/consoleOut.h>
#include <menu/IO/linuxKeyIn.h>
#include <menu/fmt/text.h>
#include <menu/fmt/titleWrap.h>
#include <menu/comp/endis.h>
#include <menu/comp/numField.h>

bool running=true;

using Out=MenuOut<
  FullPrinter<
    TitleWrapFmt<
      TextFmt<
        RangePanel<
          StaticPanel<0,0,20,4,Console>
        >
      >
    >
  >
>;

const char* op1_txt="Option 1";
const char* op2_txt="Option 2";
const char* tog_txt="Tog 1/2";
const char* subop_txt="submenu option!";
const char* op3_txt="...";
const char* quit_txt="<Quit";
const char* exit_txt="<Exit";
const char* main_txt="Main menu";
const char* sub_txt="Sub-menu";
const char* yr_txt="Year";
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

//returning false from an action will exit the current level
inline constexpr bool exit() {return false;}

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
    cout<<"Nav pos:"<<nav.pos()<<endl;
    cout<<"level:"<<nav.level<<endl;
    Ref ref=nav;
    cout<<"path:";
    for(int n=0;n<ref.len;n++)
      cout<<"/"<<nav.path[n];
    cout<<"/"<<nav.pos();
  }
};

//will togle enable/disable state of the first 2 options
bool tog12();

int year=1967;

// const char* myList[]{
//   "first item",
//   "secod item",
//   "third item",
//   "last item"
// };

using MainMenu=StaticMenu<
  StaticText<&main_txt>,
  StaticData<
    Action<EnDis<Op1>,test>,
    EnDis<MyAction<Op2>>,
    Action<StaticText<&tog_txt>,tog12>,
    NumField<StaticText<&yr_txt>,int,year,1900,2100,1>,
    // StaticText<myList>,
    Op3,
    Op3,
    Op3,
    // Debug<//will print event info if on debug mode
      StaticMenu<
        StaticText<&sub_txt>,
        StaticData<
          StaticText<&subop_txt>,
          MyAction<Op1>,
          Action<Op2,test>,
          Op3,
          Action<StaticText<&exit_txt>,exit>
        >
      >,//remove , if using Debug
    // >,
    Op3,
    Action<StaticText<&quit_txt>,quit>
  >
>;

MainMenu mainMenu;

using Nav=StaticRoot<
  StaticNavTree<MainMenu,2>
>;

Nav nav(mainMenu);

bool tog12() {
  mainMenu.enable(0,!mainMenu.enabled(0));
  mainMenu.enable(1,!mainMenu.enabled(1));
  return true;
}

int main() {
  set_conio_terminal_mode();
  Console::raw("AM5 Tests ----------------------");
  Console::nl();

  mainMenu.enable(0,true);//enable first option
  mainMenu.enable(1,false);//disable second option

  // menu------------------------
  nav.print<Out>();
  do {
    if (nav.doInput<LinuxKeyIn>()) nav.print<Out>();
  } while(running);
  Console::nl();
  reset_terminal_mode();
  return 0;
}
