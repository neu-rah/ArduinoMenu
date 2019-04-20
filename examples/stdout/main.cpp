#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/IO/stdOut.h>
#include <menu/fmt/text.h>
using namespace AM5;

////////////////////////////////////////////////////

//define a specific output
//here std::cout with a scroll panel and text format
using Out=TextFmt<//text format
  RangePanel<//provide scroll position
    StaticPanel<0,0,10,4,//output geometry
      StdOut<>//raw output
    >
  >
>;
Out out;//defning an instance for convenience.. but not needed, just for testing

template<typename T>
inline StdOut<>& operator<<(StdOut<>& o,T x) {
  o.raw(x);
  return o;
}

inline StdOut<>& operator<<(StdOut<>& o,StdOut<>&(*f)(StdOut<>&)) {
  return (*f)(o);
}

auto nl=out.endl;

template<const char** text>
using Op=EnDis<StaticText<text>>;

const char* op1_text="Op 1";
const char* op2_text="...";
const char* op3_text="Op n";
const char* menu_title="Main menu";

//menu with title
using Menu=StaticText<&menu_title,
  StaticMenu<
    Op<&op1_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op2_text>,
    Op<&op3_text>
  >
>;

using NavRoot=Nav<Menu, Out, PosDef<>>;
NavRoot nav;

int main(int,const char**) {
  out<<"AM5 std::ostream output"<<nl;
  nav.enable<2>(false);
  // nav.out(nav.getMenu());
  // out<<nl;
  // nav.getMenu().template printItems<NavRoot>();
  nav.printMenu();
  return 0;
}
