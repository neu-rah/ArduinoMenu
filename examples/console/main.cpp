#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/IO/consoleOut.h>
#include <menu/fmt/text.h>
#include <menu/comp/vectorMenu.h>

using namespace Menu;

/* TODO: test this things
+ single field menu
- multiple outputs
+ arduino and console
! static and dynamic stuff
+ use fmt's
+ use printers (as output construct)
- use panels, viewports and free outputs
- common stream operators
- minimal draw (check changed)
- parent draw (delegate draw)
- partial draw (output can draw areas)
- full draw (output draws entire page)
- raster draw (output need to be rasterized)
- viewports should void the return values on print, as they track free space
*/

using Out=TextFmt<Console<cout,FullPrinter<>>>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* extra_text="extra";
const char* menu_title="Main menu";

using MainMenu=StaticText<
  &menu_title,
  StaticList<
    StaticText<&op1_text>,
    StaticText<&op2_text>,
    StaticText<&op3_text>
  >
>;

StaticNavNode<Out,MainMenu> nav;

const char* single_text="Single field";
StaticNavNode<
  TextFmt<
    Console<cout,SinglePrinter<>>
  >,
  StaticText<&single_text>
> singleNav;

using DynMenu=VectorMenu<StaticText<&menu_title>>;

DynMenu dynMenu(
  new Prompt<StaticText<&op1_text>>(),
  new Prompt<StaticText<&op2_text>>(),
  new Prompt<StaticText<&op3_text>>()
);

NavNode<MenuOut<Out>,DynMenu> dynNav(dynMenu);

int main(int,const char**) {
  cout<<"AM5 test std::ostream"<<endl;
  cout<<endl<<"test single field"<<endl<<endl;
  singleNav.printMenu();//single elements
  cout<<endl<<"test static menu"<<endl<<endl;
  nav.printMenu();//static menu
  cout<<endl<<"test dynamic menu"<<endl<<endl;
  dynNav.printMenu();//dynamic menu
  cout<<endl<<"adding new element"<<endl<<endl;
  dynMenu.push_back(new Prompt<StaticText<&extra_text>>());
  dynNav.printMenu();//dynamic menu
  return 0;
}
