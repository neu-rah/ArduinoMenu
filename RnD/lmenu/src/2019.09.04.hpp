#include <iostream>
using namespace std;

#include <lpp.h>

using TinyMCU=lambda::True;
using idx_t=TinyMCU::Bind<lambda::Char>::Bind<lambda::As<size_t>>;

struct Atom:lambda::Nil {};

////////////////////////////////////////////////////////////////////////////////
template<typename O=Atom>
struct Void:O {
  template<typename T>
  inline static void raw(T) {}
  inline static void nl() {}
  template<typename,typename,typename> inline static void printMenu() {}
};

template<typename Dev,Dev& dev,typename O=Void<>>
struct OutStream:O {
  template<typename T>
  inline static void raw(T o) {dev<<o;}
  inline static void nl() {dev<<endl;}
};

using Console=OutStream<decltype(cout),cout>;

template<typename O>
struct StaticOut:O {
  using This=StaticOut<O>;
  template<typename N,typename I>
  inline static void printMenu(N& n,I& i) {printMenu<This,N,I>(n,i);}
};

template<typename O>
struct FullPrinter {
  template<typename _O,typename N,typename I>
  inline static void printMenu(_O& o,N& n,I& i) {

  }
};

struct IOut {
  inline virtual void raw(const char*)=0;
  inline virtual void raw(long)=0;
};

template<typename O>
struct Out:public IOut,public O {
  inline virtual void raw(const char*text) {O::raw(text);};
  inline virtual void raw(long n) {O::raw(n);};
};

////////////////////////////////////////////////////////////////////////////////
template<typename R,typename N=Atom>
struct Drift:N {
  using This=Drift<R,N>;
  template<typename Nav,typename I,typename O>
  inline static void printMenu() {O::template printMenu<Nav,I>();}
};

template<typename I,typename N=Drift<I>>
struct NavPos:N {
  idx_t pos;
};



////////////////////////////////////////////////////////////////////////////////
template<typename I=Atom>
struct Empty:I {
  template<typename O>
  inline static void printTo(O&) {}
};

template<const char**text,typename I=Empty<>>
struct StaticText:I,lambda::StaticText<text> {
  template<typename O>
  inline static void printTo() {O::raw(lambda::StaticText<text>::value());}
  template<typename O>
  inline static void printTo(O& o) {o.raw(lambda::StaticText<text>::value());}
};

struct IItem {
  virtual void printTo(IOut&)=0;
};

template<typename I>
struct Item:I,IItem {
  void printTo(IOut& O) override {I::printTo(O);}
};

const char*title="Main menu";
using Title=StaticText<&title>;

using MainMenu=lambda::List<>;

Out<Console> out;

int main() {
  cout<<"AM5 re-reload :P"<<endl;
  Console::raw("testing raw output...");
  Console::nl();
  Title::printTo(out);
  Console::nl();
  NavPos<MainMenu>::printMenu<Out<Console>>();
  return 0;
}
