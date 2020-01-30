#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>

Pair<Nil,Nil> ok;

const char* subText="Sub-menu";
Item<StaticText<&subText>::As> subTitle;

const char* sub1_text="Sub 1";
const char* sub2_text="Sub 2";
const char* subn_text="Sub...";
const char* exit_text="<Exit";

StaticData<
  Item<StaticText<&sub1_text>::As>,
  Item<StaticText<&sub2_text>::As>,
  Item<StaticText<&subn_text>::As>,
  Item<StaticText<&exit_text>::As>
> subMenu_data;

using SubMenu=Item<
  Menu<
    decltype(subTitle),
    decltype(subMenu_data),
    subTitle,
    subMenu_data
  >::As
>;

const char* mainText="Main menu";
Item<StaticText<&mainText>::As> title;

const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

StaticData<
  Item<StaticText<&op1_text>::As>,
  Item<StaticText<&op2_text>::As>,
  Item<StaticText<&opn_text>::As>,
  SubMenu,
  Item<StaticText<&opn_text>::As>,
  Item<StaticText<&quit_text>::As>
> mainMenu_data;

using MainMenu=Item<
  Menu<
    decltype(title),
    decltype(mainMenu_data),
    title,
    mainMenu_data
  >::As
>;

MainMenu mainMenu;

StaticNavRoot<Nav<MainMenu,1>::As> nav(mainMenu);

StaticMenuOut<
  FullPrinter,
  TextFmt,
  RangePanel<>::As,
  StreamOut<decltype(cout),cout>::As,
  TextMeasure<>::As
> out;

int main() {
  nav.level=1;
  nav.path[0]=3;
  nav.path[1]=0;
  nav.print(out);
  MDO<<endl;
  return 0;
}
