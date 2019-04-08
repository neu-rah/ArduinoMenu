//example for std::iostream
// compile command example
// g++ examples/mlang/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>
#include <menu/printers.h>
#include <menu/fmt/text.h>
#include <menu/fmt/debug.h>

enum LangCodes:size_t {textOk,textCancel};
Text enLang[]{"Ok","Cancel"};
Text ptLang[]{"Vá","Esquece"};

using MultiLang=AM5::Lang<Text>;
MultiLang langs(enLang);

template<LangCodes id>
using LangOp=Prompt<MultiLang::Text<langs,id,AM5::Empty>>;

MenuOut<AM5::ConsoleFmt<>::To<ConsoleOutDev<>>> consoleOut;

Prompt<Text> op1("Op 1");
Prompt<Text> op2("Op 2");
//some multilanguage options
LangOp<textOk> op3;
LangOp<textCancel> op4;

Prompt<StaticMenu<4>> mainMenu("Main menu",&op1,&op2,&op3,&op4);

int main(int,const char**) {
  cout<<"AM5 example ----"<<endl;
  consoleOut.setTarget(mainMenu);
  consoleOut.printMenu();
  cout<<"change language ----"<<endl;
  langs.setLangTable(ptLang);
  consoleOut.printMenu();
  cout<<"----"<<endl;
}
