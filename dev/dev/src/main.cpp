#include <iostream>
using namespace std;

#include <menu.h>

const char* mainText="Main menu";
StaticText<&mainText> title;

MenuOut<
  FullPrinter,
  RangePanel<>::As,
  StreamOut<decltype(cout),cout>::As,
  TextMeasure<>::As
> out;

Item<Text> op1("Option 1");
Item<Text> op2("Option 2");

#include <vector>
template<typename M>
struct ItemArray {
  template<typename I>
  struct As:I,vector<M*> {
    using vector<M*>::vector;
    template<typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Out& out,Idx idx=0,Idx top=0) {
      for(auto a:*this) {
        a->print(out);
        MDO<<endl;
      }
    }
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out) {
      if (role&(Roles::Title|Roles::Raw)) I::print(out);
      else out.printMenu(I::obj(),nav);
    }
  };
  template<typename I>
  using Template=As<I>;
};

Item<
  ItemArray<Item<Text>>::Template
> mainMenu_data{&op1,&op2};

NavRoot<> nav;

int main() {
  MDO<<"lone print:";
  op1.print(out);
  MDO<<endl;
  MDO<<"AM5 data size:"<<mainMenu_data.size()<<endl<<endl;
  title.print(out);
  MDO<<endl;
  mainMenu_data.print(nav,out);
  MDO<<endl;
  // out.printMenu(mainMenu_data);
  return 0;
}
