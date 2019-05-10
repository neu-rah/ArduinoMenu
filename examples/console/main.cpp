#include <menu.h>

using Out=RawOut<ostream&,cout>;

const char* singleOp_text="Just testing";
using SingleOp=StaticText<&singleOp_text>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
using MainMenu=StaticMenu<
  StaticText<&op1_text>,
  StaticText<&op2_text>,
  StaticText<&op3_text>
>;

const char* dynaMenu_title="Dynamic menu";
using DynaMenu=VectorMenu<StaticText<&dynaMenu_title>>;

DynaMenu dynaMenu{
  new Prompt<StaticText<&op1_text>>(),
  new Prompt<StaticText<&op2_text>>(),
  new Prompt<StaticText<&op3_text>>()
};

Out out;//to use with single option
StaticNav<Out,MainMenu> nav;
DynamicNav<MenuOutDef<Out>,DynaMenu> dyNav(dynaMenu);

int main() {
  cout<<"AM5 tests"<<endl;
  cout<<"{single option test}"<<endl;
  SingleOp::print(out);
  cout<<endl;
  cout<<"{static menu test}"<<endl;
  nav.printMenu();
  cout<<endl;
  cout<<"{dynamic menu test}"<<endl;
  dyNav.printMenu();
  cout<<endl<<"-------"<<endl;
}
