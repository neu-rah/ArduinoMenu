//example for std::iostream
// compile command example
// g++ examples/mlang/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
#include <menu/comp/multiLang.h>
using namespace Menu;

enum Langs {en,pt,cnt};

enum LangCodes:size_t {textOk,textCancel};

using MultiLang=Lang<Langs>;

template<>
const char* MultiLang::texts[][cnt]{
  {"Ok","Vá"},//textOk
  {"Cancel","Esquece"}//textCancel
};

template<LangCodes id>
using LangOp=Prompt<asTitle<MultiLang::Text<id,Text<Empty>>>>;

ConsoleOut consoleOut;

//normal option
Op op1("Op 1");

//option using flash text
Op op2("Op 2");

LangOp<textOk> op3;
LangOp<textCancel> op4;

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2,&op3,&op4};

int main(int,const char**) {
  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    consoleOut<<*ops[n];
    cout<<endl;
  }
  cout<<"change language"<<endl;
  MultiLang::sel=pt;

  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    consoleOut<<*ops[n];
    cout<<endl;
  }
}
