#include <iostream>
using namespace std;
#include <lpp.h>
// using namespace lpp;

template<typename ... OO>
using StaticMenu=lpp::List<OO...>;

struct Nil:lpp::Nil<> {};
template<typename I=Nil> struct Empty:I {};
template<typename I=Nil> struct Void:I {};

// template<const char** text,typename I=Empty<>>
// struct _StaticText:lambda::StaticText<text>,I {
//   template<typename Out> inline static void print() {Out::raw(text[0]);}
//   // template<typename Out> inline static void print(Out& out) {out.raw(text[0]);}
//   //
//   // template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
//   // inline static void print(It&,Nav&,Out& out,Ref,Idx) {print(out);}
// };

// using StaticText=lambda::Curry<_StaticText,2>;

const char* op1_txt="Option 1";
const char* op2_txt="Option 2";

using MainMenu=StaticMenu<
  lambda::StaticText<&op1_txt>,
  lambda::StaticText<&op2_txt>
>;

template<typename Dev, Dev& dev,typename O=Void<>>
struct StreamOut:O {
  inline static void nl() {dev<<"\n\r";}
  template<typename T> inline static void raw(T o) {dev<<o;}
};

using Console=StreamOut<decltype(std::cout),std::cout>;

inline void _test() {cout<<"X";}
using test=lpp::curry<void,decltype(_test),_test,0>;
//problem, this is similar to a member function as we have template arguments
//so instead of an object we need a concrete type

int main() {
  Console::raw("AM5 tests");
  Console::nl();
  lpp::Map<lpp::StaticValue<decltype(test),test>,MainMenu>::App;
  return 0;
}
