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

using MultiLang=Menu::Lang<Text>;
MultiLang langs(enLang);

template<LangCodes id>
using LangOp=Prompt<asTitle<MultiLang::Text<langs,id,Menu::Empty>>>;

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
//some multilanguage options
LangOp<textOk> op3;
LangOp<textCancel> op4;

// Menu::Item* ops[]{&op1,&op2,&op3,&op4};
Prompt<StaticMenu<4>> mainMenu("Main menu",&op1,&op2,&op3,&op4);

int main(int,const char**) {
  cout<<"AM5 example ----"<<endl;
  consoleOut.printMenu(mainMenu);
  cout<<"change language ----"<<endl;
  langs.setLangTable(ptLang);
  consoleOut.printMenu(mainMenu);
  cout<<"----"<<endl;
}
