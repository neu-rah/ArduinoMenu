#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/IO/stdOut.h>
#include <menu/fmt/text.h>
#include <menu/comp/vectorMenu.h>
using namespace AM5;

////////////////////////////////////////////////////

//define a specific output
//here std::cout with a scroll panel and text format
using Out=TextFmt<//text format
  // RangePanel<//provide scroll position
  //   StaticPanel<0,0,10,4,//output geometry
      StdOut<>//raw output
  //   >
  // >
>;

using CharPtr=Text<Empty<>>;

using Op=Prompt<CharPtr>;

//using std::vector
using Menu=Prompt<VectorMenuDef<CharPtr>>;
Menu mainMenu("Main menu"
  ,new Op("Op 1")
  ,new Op("Op 2")
  ,new Op("Op 3")
  ,new Op("on heap")
);

Op extra("extra option, dynamic add");

using NavRoot=Nav<Menu, Out, PosDef<>>;
NavRoot nav;

int main(int,const char**) {
  nav<<"AM5 std::ostream output"<<nav.endl;
  nav.enable<2>(false);
  nav.printMenu();
  return 0;
}
