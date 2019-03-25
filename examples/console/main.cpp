//example for std::iostream
// compile command example
// g++ examples/console/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
using namespace Menu;

ConsoleOut consoleOut;

//normal option
Op op1("Op 1");

//option using flash text
Op op2("Op 2");

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2};

int main(int,const char**) {
  for(int n=0;n<sizeof(ops)/sizeof(Item*);n++) {
    consoleOut<<*ops[n];
    cout<<endl;
  }
}
