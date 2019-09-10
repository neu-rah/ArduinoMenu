#include <iostream>
using namespace std;

#include <lpp.h>
////////////////////////////////////////////////////////////////////////////////
struct Void {
  template<typename T>
  inline static void raw(T o) {}
  inline static void nl() {}
  inline static void printMenu() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue() {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit() {}
};

template<typename Dev,Dev& dev,typename O=Void>
struct StreamOut {
  template<typename T>
  inline static void raw(T o) {dev<<o;}
  inline static void nl() {dev<<endl;}
};

using Console=StreamOut<decltype(cout),cout>;

////////////////////////////////////////////////////////////////////////////////
struct Drift {
  inline static void enter(){}
  inline static void esc(){}
  inline static void up(){}
  inline static void down(){}
  inline static void left(){}
  inline static void right(){}
  template<typename Nav>
  inline static void printMenu() {}
};

template<typename Data,typename Nav>
struct StaticNav:Drift {
  static Nav nav;
};

////////////////////////////////////////////////////////////////////////////////
struct Empty {
  template<typename Out>
  inline static void printTo() {}
};

template<const char** text,typename Item=Empty>
struct StaticText:Item,lambda::StaticText<text> {
  using App=typename lambda::StaticText<text>::App;
  template<typename Out>
  inline static void printTo() {
    Out::raw(lambda::StaticText<text>::value());
    Item::template printTo<Out>();
  }
};

template<typename I,typename... II>
struct StaticMenu:I {
  using This=StaticMenu<I,II...>;
  using Head=I;
  using Tail=StaticMenu<II...>;
  template<typename O>
  inline static void printTo() {
    Head::template printTo<O>();
    Tail::template printTo<O>();
  }
};

template<typename I>
struct StaticMenu<I>:I {
  template<typename O>
  inline static void printTo() {
    I::template printTo<O>();
  }
};

////////////////////////////////////////////////////////////////////////////////

const char* op1_text="Option 1";
const char* op2_text="Option 2";

const char* mainMenu_title="Main menu";
using MainMenu_title=
  StaticText<
    &mainMenu_title,
    StaticMenu<
      StaticText<&op1_text>,
      StaticText<&op2_text>
    >
  >;

int main() {
  MainMenu_title::printTo<Console>();
  Console::nl();
  return 0;
}
