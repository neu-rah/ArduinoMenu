#include <iostream>
using namespace std;

#include "chain.h"

template<typename O>
struct CRTP {
  using Type=O;
  O& obj() {return *(O*)this;}
};

template<typename I>
struct Empty:I {};

template<typename I>
struct Text:I {
  void test() {I::obj().top();}
};

template<Expr... I>
struct Item:Chain<I...>::template To<CRTP<Item<I...>>> {
  void top() {cout<<"Ok!"<<endl;}
};

using X=Item<Text,Empty>;

X x;

int main() {
  x.test();
  return 0;
}
