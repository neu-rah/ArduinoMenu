#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>

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

NavRoot<Nav<MainMenu,2>::Part> nav(mainMenu);

MenuOut<
  FullPrinter,
  TextFmt,
  RangePanel<>::Part,
  StreamOut<decltype(cout),cout>::Part,
  TextMeasure<>::Part
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
  nav.path[0]=4;
  nav.level=1;
  nav.path[1]=0;
  nav.print(out);
  // mainMenu.print(nav,out);
  MDO<<endl;
  return 0;
}
