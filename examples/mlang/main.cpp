//example for std::iostream
// compile command example
// g++ examples/mlang/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>

enum LangCodes:size_t {textOk,textCancel};

Text enLang[]{"Ok","Cancel"};
Text ptLang[]{"Vá","Esquece"};

using MultiLang=Menu::Lang<Text>;
MultiLang langs(enLang);

template<LangCodes id>
using LangOp=Prompt<asTitle<MultiLang::Text<langs,id>>>;

ConsoleOut consoleOut;

//normal option
Prompt<Op> op1("Op 1");

//option using flash text
Prompt<Op> op2("Op 2");

LangOp<textOk> op3;
LangOp<textCancel> op4;

//they can fit on same array
//and will preserve the composed behavior
Menu::Item* ops[]{&op1,&op2,&op3,&op4};

int main(int,const char**) {
  cout<<"AM5 example ----"<<endl;
  for(auto o: ops) {
    consoleOut<<*o;
    cout<<endl;
  }
  cout<<"change language ----"<<endl;
  langs.setLangTable(ptLang);

  for(auto o: ops) {
    consoleOut<<*o;
    cout<<endl;
  }
  cout<<"----"<<endl;
}
