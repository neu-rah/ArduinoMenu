//example for std::iostream
// compile command example
// g++ examples/console/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>
#include <menu/printer/full.h>
#include <menu/fmt/text.h>
#include <menu/fmt/debug.h>

Menu::MenuOutCap<
  Menu::DebugFmt<//add debug info to output (if enabled)
    Menu::FullPrinter<//print innet then options
      Menu::TitlePrinter<//print the title
        Menu::TextFmt<//text format, insert \n at item or title end, etc...
          Menu::WrapTitle<//print title surrounded by []
            ConsoleOut//standard output
          >
        >
      >
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
  consoleOut.printMenu(mainMenu);
}
