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

template<typename I=Empty<>>
struct Text:I {
  const char* text;
  inline Text(const char*t):text(t) {}
  template<typename Out> inline void print(Out& out) {out.raw(text);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void print(It& it,Nav& nav,Out& out) {print(out);}
};

bool quit() {
  //just signal program exit
  _trace(MDO<<"Quit!"<<endl);
  running=false;
  return true;
}

IItem* subMenu_data[] {
  new Item<Text<>>("A"),
  new Item<Text<>>("mA"),
  new Item<Text<>>("uA")
};

Item<Text<>> subMenu_title("sub menu");
Item<Menu<>> subMenu(subMenu_title,subMenu_data,sizeof(subMenu_data)/sizeof(IItem*));

//heap static allocation
IItem* mainMenu_data[] {
  new Item<StaticText<&op1_txt>>(),
  new Item<StaticText<&op2_txt>>(),
  new Item<StaticText<&opn_txt>>(),
  new Item<StaticText<&opn_txt>>(),
  new Item<Text<>>("Ok"),
  new Item<Text<>>("Some other option"),
  &subMenu,
  new Item<Action<StaticText<&quit_txt>,quit>>
};

Item<StaticText<&title_txt>> mainTitle;
Item<Menu<>> mainMenu(mainTitle,mainMenu_data,sizeof(mainMenu_data)/sizeof(IItem*));

NavRoot<NavTree<2>> nav(mainMenu);

int main() {
  Console::raw("AM5 Tests ----------------------");
  Console::nl();

  nav.up();
  // menu------------------------
  nav.printOn(out);
  do {
    if (nav.doInput(in)) nav.printOn(out);
    cout.flush();
  } while(running);
  Console::nl();
  return 0;
}
