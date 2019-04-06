// AM5 example for std::iostream
// compile command example
// g++ examples/console/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>

MenuOut<Menu::ConsoleFmt<>::To<ConsoleOutDev<>>> consoleOut;

using Op=Prompt<Text>;
Prompt<Op> op2("Op 2");
Prompt<StaticMenu<4>> mainMenu("Main menu"
  ,new Op("Op 1")
  ,new Op("Op 2")
  ,new Op("Op 3")
  ,new Prompt<Op>("on heap")
);

int main(int,const char**) {
  consoleOut.setTarget(mainMenu);
  consoleOut.printMenu();
  return 0;
}
