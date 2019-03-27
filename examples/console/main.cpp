//example for std::iostream
// compile command example
// g++ examples/console/main.cpp -o am5 -I src -std=c++11

#include <menu/def/console.h>
using namespace Menu;

ConsoleOut consoleOut;

//normal option
Prompt<Op> op1("Op 1");

//option using flash text
Prompt<Op> op2("Op 2");

//they can fit on same array
//and will preserve the composed behavior
Item* ops[]{&op1,&op2};

int main(int,const char**) {
  for(auto o : ops) {
    consoleOut<<*o;
    cout <<endl;
  }
}
