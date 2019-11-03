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
Out out;

MenuIn<LinuxKeyIn> in;

const char* title_txt="Main menu";
const char* op1_txt="Option 1";
const char* op2_txt="Option 2";
const char* opn_txt="Op...";
const char* quit_txt="<Quit";

template<typename I>
struct Text:I {
  const char* text;
  template<typename Out> inline void print(Out& out) {out.raw(text);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void print(It& it,Nav& nav,Out& out) {print(out);}
};

bool quit() {
  //just signal program exit
  return running=false;
}

//heap static allocation
IItem* mainMenu_data[] {
  new Item<StaticText<&op1_txt>>(),
  new Item<StaticText<&op2_txt>>(),
  new Item<StaticText<&opn_txt>>(),
  new Item<StaticText<&opn_txt>>(),
  new Item<Action<StaticText<&quit_txt>,quit>>
};

Item<StaticText<&title_txt>> mainTitle;
Item<Menu<>> mainMenu(mainTitle,mainMenu_data,sizeof(mainMenu_data)/sizeof(IItem*));

NavRoot<NavTree<1>> nav(mainMenu);

int main() {
  Console::raw("AM5 Tests ----------------------");
  Console::nl();

  // menu------------------------
  nav.printOn(out);
  nav.up();
  do {
    if (nav.doInput(in)) nav.printOn(out);
    cout.flush();
  } while(running);
  Console::nl();
  return 0;
}
