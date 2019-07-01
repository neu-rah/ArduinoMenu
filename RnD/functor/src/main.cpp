#include <iostream>
using namespace std;

template<auto o> struct StaticData {};
template<auto o> inline auto id() {return o;}
template<auto o> inline auto pure() {return o;}
template<auto o> auto pure<StaticData<o>>() {return StaticData<o>();}

auto add(auto a){return [a](auto b){return pure<a+b>();};}

auto fmap(auto f){
  return [f](auto o){return f();};
}

StaticData<2> a;

int main() {
  cout<<add(2)(3);
  return 0;
}
