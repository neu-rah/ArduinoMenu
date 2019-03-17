#include <iostream>
using namespace std;

template<typename B>
struct C:public vitual B {
  struct I {};

  template<typename O>
  struct A:public virtual I,public virtual O {};
};

int main(int argc, char** argv) {
  cout<<"AM5 test ------------------"<<endl;
  cout<<"-------"<<endl;
  return 0;
}
