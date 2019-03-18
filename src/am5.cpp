#include <iostream>
#include <vector>
using namespace std;

using Out=ostream;

//////////////////////////////////////////////////
// interface
struct Base {
  inline virtual Out& operator<<(Out& o) const {return o;}
  inline virtual size_t size() const {return 0;}
  inline virtual Base& operator[](size_t n) const {throw 1;}
};

//adapter
template<typename O>
struct Item:public Base,public O {
  using O::O;

  template<typename... OO>
  inline Item(OO... oo):O(oo...) {}
  template<typename... OO>
  inline Item(const char*title,OO... oo):O(title,oo...) {}
  inline Item(const char*title):O(title) {}

  inline Out& operator<<(Out& o) const override {return O::out(o);}
  inline size_t size() const override {return O::size();}
  inline Base& operator[](size_t n) const override {return O::operator[](n);}
};

inline Out& operator<<(Out& out,Base& item) {return item.operator<<(out);}

/////////////////////////////////////////////////////////
//static routers
struct Empty {
  static inline Out& out(Out& o) {return o;}
  static inline size_t size() {return 0;}
  inline Base& operator[](size_t n) const {throw 1;}
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
  Text(const char* t):text(t) {}
  inline Out& out(Out& o) const {return o<<text;}
};

template<size_t n,typename O=Empty>
class StaticMenu:public O {
protected:
  Base* data[n];
public:
  template<typename... OO>
  inline StaticMenu(OO... oo):data{oo...} {}
  template<typename... OO>
  inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
  static inline size_t size() {return n;}
  inline Base& operator[](size_t i) const {return *data[i];}
};

template<typename O=Empty>
class VectorMenu:public O {
protected:
  vector<Base*> data;
public:
  VectorMenu(initializer_list<Base*> oo):data(oo) {}
  VectorMenu(const char*title,initializer_list<Base*> oo):O(title),data(oo) {}
  template<typename... OO>
  VectorMenu(const char*title,OO... oo):O(title),data{oo...} {}
  template<typename... OO>
  VectorMenu(OO... oo):data{oo...} {}
  inline size_t size() const {return data.size();}
  inline Base& operator[](size_t n) const {return *data[n];}
};

const char* op1_text="op 1";
Item<StaticText<&op1_text>> op1;

const char* op2_text="op 2";
Item<StaticText<&op2_text>> op2;

Item<Text<>> op3("op 3");

const char* aTitle="a title!";
const char* staticMenu_staticTitle_text="StaticMenu with static title";
Item<StaticMenu<3,StaticText<&staticMenu_staticTitle_text>>> staticMenu_staticTitle(&op1,&op2,&op3);
Item<StaticMenu<3,Text<>>> staticMenu_dynTitle("staticMenu with dyn. title",&op1,&op2,&op3);
Item<StaticMenu<3>> staticMenu_noTitle(&op1,&op2,&op3);
Item<VectorMenu<StaticText<&aTitle>>> vectorMenu_staticTitle(&op1,&op2,&op3);
Item<VectorMenu<Text<>>> vectorMenu_dynTitle("vector menu with dyn. title",&op1,&op2,&op3);
Item<VectorMenu<>> vectorMenu_noTitle(&op1,&op2,&op3);

//using some sugar
using Menu=Item<VectorMenu<Text<>>>;
using Op=Item<Text<>>;

Menu mainMenu("Main menu"
  ,new Op("Ok")
  ,&staticMenu_staticTitle
  ,&staticMenu_dynTitle
  ,&staticMenu_noTitle
  ,&vectorMenu_staticTitle
  ,&vectorMenu_dynTitle
  ,&vectorMenu_noTitle
);

struct Core {
  Out& printMenu(Out& out,Base& menu) {
    out<<menu<<endl;//this `endl`s will be compose on out device style instead!
    for(int n=0;n<menu.size();n++)
      out<<menu[n]<<endl;
    return out;
  }
} core;

int main(int argc, char** argv) {
  cout<<"AM5 test ------------------"<<endl;
  core.printMenu(cout,mainMenu);
  cout<<"-------"<<endl;
  return 0;
}
