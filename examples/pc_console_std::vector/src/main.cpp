#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>
#include <menu/IO/linuxKeyIn.h>

bool running=true;

//menu data/texts ----------------------------
const char* mainText="Main menu";
Item<StaticText<&mainText>::Part> title;

Prompt<Text> op1("Option 1");
Prompt<Text> op2("Option 2");
Prompt<Text> opn("Option...");
Prompt<Text> quit("<Quit.");

Prompt<Text> sub_title("Sub-menu");
Prompt<Text> sub1("Sub 1");
Prompt<Text> sub2("Sub 2");
Prompt<Text> subn("Sub...");
Prompt<Text> exitOp("<Exit");

//menu structure -------------------------
Prompt<
  ItemArray<IItem>::Part
> subMenu_data{
  &sub1,
  &sub2,
  &subn,
  &exitOp
};

using SubMenu=Prompt<
  Menu<
    decltype(sub_title),
    decltype(subMenu_data),
    sub_title,
    subMenu_data
  >::Part
>;

SubMenu subMenu;

Prompt<
  ItemArray<IItem>::Part
> mainMenu_data{
  &op1,
  &op2,
  &opn,
  &opn,
  (IItem*)&subMenu,
  &quit
};

using MainMenu=Prompt<
  Menu<
    decltype(title),
    decltype(mainMenu_data),
    title,
    mainMenu_data
  >::Part
>;

MainMenu mainMenu;

//menu navigation control -------------------------
NavRoot<Nav<MainMenu,2>::Part> nav(mainMenu);

//menu output --------------------------------------
MenuOut<
  FullPrinter,
  TextFmt,
  RangePanel<>::Part,
  StreamOut<decltype(cout),cout>::Part,
  TextMeasure<>::Part
> out;

//menu input --------------------------------------
LinuxKeyIn in;

int main() {
  nav.print(out);
  while(running) {
    if (nav.doInput(in)) {
      nav.print(out);
      out.nl();
    }
    cout.flush();
  };
  return 0;
}
