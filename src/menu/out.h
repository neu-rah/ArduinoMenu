/* -*- C++ -*- */
#pragma once

#include "base.h"

//////////////////////////////////////////////
template<typename O>
struct Void:public O {
  template<typename T>
  static inline void raw(T o) {}
  static inline void nl() {}
  static inline void clrLine(idx_t) {}
  template<typename N,typename H,typename I>
  static void print(N,H,I) {}
  constexpr static inline bool isRange() {return true;}
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline idx_t height() {return 0;}
  constexpr static inline idx_t top() {return 0;}
  static inline void setTop(idx_t) {}
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
};

template<typename Dev,Dev& dev,typename O=Void<>>
struct StaticOut:public O {
  static inline void nl() {dev<<endl;}
  template<typename T>
  inline void raw(T o) {dev<<o;}
  template<typename N,typename H,typename I>
  inline void print(N n,H o,I i) {i.print(n,o);}
};

template<ostream& dev,typename O=Void<>>
using Console=StaticOut<ostream,dev,O>;

template<typename O=Void<>>
using StdOut=Console<cout,O>;

struct IMenuOut {
  virtual void raw(const char* o)=0;
  virtual inline void print(INavNode& n,IItem& i)=0;
};

template<typename O>
struct MenuOut:public O {
  void raw(const char* o) override {O::raw(o);}
  void print(INavNode& n,IItem& i) override {i.print(n,*this,i);}
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
