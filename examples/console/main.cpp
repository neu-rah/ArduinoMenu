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

using Out=TextFmt<Console<cout,FullPrinter<>>>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* menu_title="Main menu";

using MainMenu=StaticText<
  &menu_title,
  StaticList<
    StaticText<&op1_text>,
    StaticText<&op2_text>,
    StaticText<&op3_text>
  >
>;

NavNode<Out,MainMenu> nav;

const char* single_text="Single field";
NavNode<
  TextFmt<
    Console<cout,SinglePrinter<>>
  >,
  StaticText<&single_text>
> singleNav;

int main(int,const char**) {
  cout<<"AM5 test std::ostream"<<endl;
  cout<<endl<<"test single field"<<endl<<endl;
  singleNav.printMenu();//single elements
  cout<<endl<<"test static menu"<<endl<<endl;
  nav.printMenu();//static menu
  return 0;
}
