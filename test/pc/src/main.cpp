//this is part of development files and might not work
//its just for testing and experimenting

#include <iostream>
using namespace std;

// #include <lpp.h>
// using namespace lpp;

template<const unsigned char b>
void test() {cout<<"using "<<b<<endl;}

//here using a runtime value to seek a constexpr without having to extensivelly enumerate all cases...
// however compiler will enumerate them (just less typing)
//still a forward only api, we can not return 
template<unsigned char o=0>
void byte(unsigned char p) {
  if(o==p) test<o>();
  else byte<o+1>(p);
}

int main() {
  const char x=cin.get();
  byte(x);
  return 0;
}
