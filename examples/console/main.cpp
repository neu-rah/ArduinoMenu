// AM5 example for std::iostream
// compile command example
// g++ examples/console/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>
#include <menu/printers.h>
#include <menu/nav.h>
#include <menu/fmt/text.h>
#include <menu/fmt/debug.h>
#include <menu/fmt/titleWrap.h>

template<typename O>
using WrapTitle=Menu::TitleWrapFmt<O>;

MenuOut<//menu output
  Menu::Chain<//wrap inner types
    Menu::DebugFmt,//add debug info when enabled
    Menu::TextFmt,//text format, insert \n at item or title end, etc...
    WrapTitle,//surround titles with []
    Menu::FullPrinter,//print inner then options
    Menu::NavNode,//flat navigation control (no sub menus)
    ConsoleOut//use std default console (cout)
  >::To<//device parts to be used for panel|menu|title|item
    Menu::DeviceParts<
      //install format message emitter for items,titles,menu and panel, use Menu::ID to ommit the parts
      Menu::Chain<Menu::TextAccelPrinter,Menu::TextCursorPrinter,Menu::ItemPrinter>::To,//emit format messages for accel, cursor amd item
      Menu::TitlePrinter,//emit format messages for titles (fmtTitle)
      Menu::ID,//menu parfts (not used yet)
      Menu::ID//panel parts (not used yet)
    >
  >
> consoleOut;

Prompt<Op> op1("Op 1");
Prompt<Op> op2("Op 2");
Prompt<StaticMenu<3>> mainMenu("Main menu",
  &op1,
  &op2,
  new Prompt<Op>("on heap")
);

int main(int,const char**) {
  consoleOut.setTarget(mainMenu);
  consoleOut.printMenu();
}
