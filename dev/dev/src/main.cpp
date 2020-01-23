#include <iostream>
using namespace std;

#include <menu.h>
#include <menu/comp/vector.h>
#include <menu/fmt/fullText.h>

const char* mainText="Main menu";
Item<StaticText<&mainText>::As> title;

Item<Text> op1("Option 1");
Item<Text> op2("Option 2");

Item<
  ItemArray<Item<Text>>::Open
> mainMenu_data{&op1,&op2};

NavRoot<> nav;

MenuOut<
  FullPrinter,
  TextFmt,
  RangePanel<>::As,
  StreamOut<decltype(cout),cout>::As,
  TextMeasure<>::As
> out;

StaticMenu<decltype(title),decltype(mainMenu_data),title,mainMenu_data>::template As<> mainMenu;

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
  out.printMenu(mainMenu,nav);
  MDO<<endl;
  return 0;
}
