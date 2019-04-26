#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/IO/console.h>
#include <menu/fmt/text.h>

using namespace Menu;

/* TODO: test this things
- single field menu
- multiple outputs
- arduino and console
- static and dynamic stuff
+ use fmt's
+ use printers (as output construct)
- use panels, viewports and free outputs
- common stream operators
- minimal draw (check changed)
- parent draw (delegate draw)
- partial draw (output can draw areas)
- full draw (output draws entire page)
- raster draw (output need to be rasterized)
*/

using Out=ItemPrinter<
  TextFmt<
    Console<cout,PrintersCfg<>>
  >
>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";

using MainMenu=StaticList<
  StaticText<&op1_text>,
  StaticText<&op2_text>,
  StaticText<&op3_text>
>;

using Nav=NavNode<Out,MainMenu>;
Nav nav;

int main(int,const char**) {
  cout<<"AM5 test std::ostream"<<endl;
  nav.printMenu();
  return 0;
}
