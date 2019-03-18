#include <iostream>
#include <vector>
using namespace std;

using Out=ostream;

//////////////////////////////////////////////////
// interface
struct Base {
  inline virtual Out& operator<<(Out& o) const {return o;}
};

//adapter
template<typename O>
struct Item:public Base,public O {
  using O::O;
  inline Out& operator<<(Out& o) const override {return O::out(o);}
};

/////////////////////////////////////////////////////////
//static routers

struct Empty {
  static inline Out& out(Out& o) {return o;}
};

template<const char** text,typename O=Empty>
struct StaticText:public O {
  static inline Out& out(Out& o) {return o<<text[0];}
};

template<typename O=Empty>
class Text:public O {
protected:
  const char *text;
public:
  Text():text("") {}
  Text(const char* t):text(t) {}
  inline Out& out(Out& o) const {return o<<text;}
};

template<size_t n,typename O=Empty>
class StaticMenu:public O {
protected:
  Base* data[n];
public:
  template<typename... OO>
  StaticMenu(OO... oo):data{oo...} {}
  template<typename... OO>
  StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
};

template<typename O=Empty>
class VectorMenu:public virtual O {
protected:
  vector<Base*> data;
public:
  VectorMenu(initializer_list<Base*> oo):data(oo) {}
  VectorMenu(const char*title,initializer_list<Base*> oo):O(title),data(oo) {}
  template<typename... OO>
  VectorMenu(const char*title,OO... oo):O(title),data{oo...} {}
  template<typename... OO>
  VectorMenu(OO... oo):data{oo...} {}
};

const char* op1_text="op 1";
Item<StaticText<&op1_text>> op1;

const char* op2_text="op 2";
Item<StaticText<&op2_text>> op2;

Item<Text<>> op3("op 3");

const char* aTitle="a title!";
Item<StaticMenu<2,StaticText<&aTitle>>> staticMenu_staticTitle(&op1,&op2);
Item<StaticMenu<2,Text<>>> staticMenu_dynTitle("staticMenu with dyn. title",&op1,&op2);
Item<VectorMenu<StaticText<&aTitle>>> vectorMenu_staticTitle(&op1,&op2);
Item<VectorMenu<Text<>>> vectorMenu_dynTitle("vector menu with dyn. title",&op1,&op2);

int main(int argc, char** argv) {
  cout<<"AM5 test ------------------"<<endl;
  // cout<<mainMenu<<endl;
  // cout<<mainMenu.data[0]<<endl;
  // cout<<mainMenu.data[1]<<endl;
  cout<<"-------"<<endl;
  return 0;
}
