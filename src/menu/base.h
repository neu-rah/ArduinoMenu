/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu interfaces (API's)
*/

#ifndef ARDUINO
  #include <string>
  // using namespace std;
#endif

//type nesting from a list of templates+defered terminal type
#define Expr template<typename> class
#define Term typename

template<Expr... OO>
struct Chain {
  template<Term _T,Expr _O, Expr... _OO>
  struct Links:public _O<Links<_T,_OO...>> {using _O<Links<_T,_OO...>>::_O;};

  template<Term _T,Expr _O>
  struct Links<_T,_O>:public _O<_T> {using _O<_T>::_O;};

  template<Term T>
  struct To:public Links<T,OO...> {};
  //provide a continuation
  template<Expr _O>
  using With=Chain<OO...,_O>;
};

#undef Expr
#undef Term

using idx_t=int;
using ConstText=const char[];

enum class Roles {Panel,Menu,Title,Body,Item,Index,Cursor,Name,Mode,Value,Unit};

// //turn enum into type for template specialization
// template<Roles role> struct Role {};

//Identity, for when we need a template instead of a type
template<typename O> using Id=O;

//just and empty terminator
struct Nil {};

struct Item;
struct MenuOut;
struct NavNode;

//Output Device Operation
enum class OutOps {RawOut,Measure};
template<OutOps>
struct OutOp {};

using RawOutOp=OutOp<OutOps::RawOut>;
using MeasureOp=OutOp<OutOps::Measure>;

//navigation interface
struct NavNode {
  virtual inline bool selected(idx_t) const {return false;}
  virtual inline bool enabled(idx_t) const {return true;}
  virtual inline bool up()=0;
  virtual inline bool down()=0;
  virtual inline bool left()=0;
  virtual inline bool right()=0;
  virtual inline bool enter()=0;
  virtual inline bool esc()=0;
};

//output interface
struct MenuOut {
  virtual inline void nl() {}
  virtual inline void raw(char) {};
  virtual inline void raw(const char*) {};
  virtual inline void raw(int) {};
  virtual inline void raw(unsigned int) {};
  virtual inline void raw(long) {};
  virtual inline void raw(unsigned long) {};
  virtual inline void raw(double) {};
  virtual inline void printItem(NavNode&,Item&,idx_t)=0;
  //reduce vtable, using a bool parameter and demux on static side
  virtual inline void fmt(Roles role,bool io,NavNode& nav,MenuOut&,Item& i,idx_t) {}
  //just a comodity to inline a role (open/close)
  inline void fmt(Roles role,NavNode& nav,MenuOut& out,Item& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
};

//item interface
struct Item {
  virtual inline void print(NavNode&,MenuOut& out) {}
  virtual inline void printItem(NavNode&,MenuOut& out,idx_t n) {}
  virtual inline void enable(idx_t,bool) {}
  virtual inline bool enabled(idx_t) const {return true;}
  virtual inline bool activate() {};
};

//static output base
template<typename O=Nil> struct Void:public O {
  static inline void nl() {}
  static inline void setCursor(idx_t x,idx_t y) {}
  template<typename Out>
  static inline void clrLine(Out&,idx_t) {}
  constexpr static inline bool isRange() {return false;}//TODO: do we need this here?
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline idx_t height() {return 0;}
  constexpr static inline idx_t top() {return 0;}
  static inline void setTop(idx_t) {}
  static inline void newView() {nl();}
  constexpr static inline idx_t posX() {return 0;}
  constexpr static inline idx_t posY() {return 0;}
  constexpr static inline idx_t freeX() {return INT16_MAX;}
  constexpr static inline idx_t freeY() {return INT16_MAX;}
  constexpr static inline idx_t free() {return INT16_MAX;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}

  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit(Nav&,Out&,I&,idx_t) {}

  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,Nav& nav,Out& out,I& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,bool io,Nav& nav,Out& out,I& i,idx_t n) {
    //we could do better with templates, but we need this to be compatible with virtual interface too
    switch(role) {
      case Roles::Panel: io?out.template fmtPanel<true>(nav,out,i,n):out.template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?out.template fmtMenu<true>(nav,out,i,n):out.template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?out.template fmtTitle<true>(nav,out,i,n):out.template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?out.template fmtBody<true>(nav,out,i,n):out.template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?out.template fmtItem<true>(nav,out,i,n):out.template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?out.template fmtIndex<true>(nav,out,i,n):out.template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?out.template fmtCursor<true>(nav,out,i,n):out.template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?out.template fmtName<true>(nav,out,i,n):out.template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?out.template fmtMode<true>(nav,out,i,n):out.template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?out.template fmtValue<true>(nav,out,i,n):out.template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?out.template fmtUnit<true>(nav,out,i,n):out.template fmtUnit<false>(nav,out,i,n);break;
    }
  }
};

//text output measure
struct TextMeasure:public Void<> {
  template<typename T>
  static inline idx_t measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  protected:
    #ifndef ARDUINO
    static inline idx_t _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline idx_t _str(T i){return std::string(std::to_string(i)).length();}
    #endif
};

template<> idx_t TextMeasure::measure<const char>(const char o) {return 1;}
// template<> idx_t TM::measure<const char*>(const char* o) {return strlen(o);}
