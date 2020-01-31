#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>
#include <menu/fmt/titleWrap.h>

Pair<Nil,Nil> ok;

const char* subText="Sub-menu";
const char* sub1_text="Sub 1";
const char* sub2_text="Sub 2";
const char* subn_text="Sub...";
const char* exit_text="<Exit";

const char* mainText="Main menu";
const char* op1_text="Option 1";
const char* op2_text="Option 2";
const char* opn_text="Option...";
const char* quit_text="<Quit.";

using MainMenu=Item<
  StaticMenu<
    StaticText<&mainText>::Part<>,
    StaticData<
      Item<StaticText<&op1_text>::Part>,
      Item<StaticText<&op2_text>::Part>,
      Item<StaticText<&opn_text>::Part>,
      Item<
        StaticMenu<
          StaticText<&subText>::Part<>,
          StaticData<
            Item<StaticText<&sub1_text>::Part>,
            Item<StaticText<&sub2_text>::Part>,
            Item<StaticText<&subn_text>::Part>,
            Item<StaticText<&exit_text>::Part>
          >
        >::Part
      >,
      Item<StaticText<&opn_text>::Part>,
      Item<StaticText<&quit_text>::Part>
    >
  >::Part
>;

MainMenu mainMenu;

StaticNavRoot<Nav<MainMenu,1>::Part> nav(mainMenu);

StaticMenuOut<
  FullPrinter,
  TitleWrapFmt<>::Part,
  TextFmt,
  RangePanel<>::Part,
  StreamOut<decltype(cout),cout>::Part,
  TextMeasure<>::Part
> out;

// using X=MainMenu::Test;

int main() {
  // nav.level=1;
  // nav.path[0]=3;
  // nav.path[1]=0;
  nav.print(out);
  MDO<<endl;
  return 0;
}
