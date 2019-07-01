#include <iostream>
using namespace std;

template<typename O,O o>
struct Static {
  static inline Static<O,o> pure(O) {return Static<O,o>();}
};

//Applicative
//this is only possible with c++17
template<typename O,O o,typename F>
auto fmap(F& f,Static<O,o>) /*->Static<decltype(f(o)),f(o)>*/ {
  // return Static<decltype(f(o)),f(o)>::pure(o);//this is stupid for static data
  return Static<decltype(f(o)),f(o)>();
}

template<typename O,O o>
void out(Static<O,o>) {cout<<"Static "<<o;}

auto id=[&](auto o){return o;};
auto dbl=[&](auto x){return x*2;};
auto lsb=[&](auto x){return x&1;};
//curry
auto shl=[&](auto x){return [&](char n=1){return x<<n;};};
auto shr=[&](auto x){return [&](char n=1){return x>>n;};};

auto add=[&](auto a){return [&](auto b){return a+b;};};
auto sub=[&](auto a){return [&](auto b){return a-b;};};
auto mul=[&](auto a){return [&](auto b){return a*b;};};
auto ratio=[&](auto a){return [&](auto b){return a/b;};};

//function application
auto run=[&](auto f){return f();};

// Static<int,3> x;
// int main() {
//   out(fmap(dbl,x));
//   cout<<endl;
//   out(fmap(lsb,x));
//   cout<<endl;
//   out(fmap(run,fmap(shl,x)));// and then it can not be applicative because templates accept no lambda.. this sucks
//   cout<<endl;
//   return 0;
// }
