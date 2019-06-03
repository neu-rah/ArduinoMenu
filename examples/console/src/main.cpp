#include <iostream>
using namespace std;

#include <menu.h>

/////////////////////////////////////////////////////////
const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* mainMenu_title="Main menu";

using Op1=Item<StaticText<&op1_text>>;

Op1 op1;

StdOut out;

Item<
  StaticText<
    &mainMenu_title,
    StaticMenu<
      Item<StaticText<&op1_text>>,
      Item<StaticText<&op2_text>>,
      Item<StaticText<&op3_text>>
    >
  >
> mainMenu;

//-----------------------
int main() {
  cout<<"PIO Native program"<<endl;
  op1.print(out);
  cout<<endl;

  mainMenu.print(out);
  return 0;
}
