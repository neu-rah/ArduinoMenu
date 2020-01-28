#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>

const char* mainText="Main menu";
Item<StaticText<&mainText>::As> title;

Prompt<Text> op1("Option 1");
Prompt<Text> op2("Option 2");
Prompt<Text> opn("Option...");
Prompt<Text> quit("<Quit.");

Prompt<Text> sub_title("Sub-menu");
Prompt<Text> sub1("Sub 1");
Prompt<Text> sub2("Sub 2");
Prompt<Text> subn("Sub...");
Prompt<Text> exitOp("<Exit");

Prompt<
  ItemArray<IItem>::As
> subMenu_data{
  &sub1,
  &sub2,
  &subn,
  &exitOp
};

using SubMenu=Prompt<
  StaticMenu<
    decltype(sub_title),
    decltype(subMenu_data),
    sub_title,
    subMenu_data
  >::As
>;

SubMenu subMenu;

Prompt<
  ItemArray<IItem>::As
> mainMenu_data{
  &op1,
  &op2,
  &opn,
  &opn,
  (IItem*)&subMenu,
  &quit
};

using MainMenu=Prompt<
  StaticMenu<
    decltype(title),
    decltype(mainMenu_data),
    title,
    mainMenu_data
  >::As
>;

MainMenu mainMenu;

NavRoot<Nav<MainMenu,1>::As> nav(mainMenu);

MenuOut<
  FullPrinter,
  TextFmt,
  RangePanel<>::As,
  StreamOut<decltype(cout),cout>::As,
  TextMeasure<>::As
> out;

int main() {
  // MDO<<"lone print:";
  // op1.print(out);
  // MDO<<endl;
  // MDO<<"AM5 data size:"<<mainMenu_data.size()<<endl<<endl;
  // title.print(out);
  // MDO<<endl;
  // mainMenu_data.printItems(nav,out);
  // MDO<<endl;
  // out.printMenu(mainMenu_data,nav);
  // MDO<<endl;
  // out.printMenu(mainMenu,nav);
  nav.print(out);
  // mainMenu.print(nav,out);
  MDO<<endl;
  return 0;
}
