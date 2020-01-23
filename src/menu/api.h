/* -*- C++ -*- */
#pragma once

#include "base.h"

////////////////////////////////////////////////////////////////////////////////
//implement CRTP
// otherwise encapsulation becomes a code issue, making customizations hard
template<typename T>
struct Obj {
  using Type=T;
  inline Type& obj() const {return *(Type*)this;}
};

////////////////////////////////////////////////////////////////////////////////
//basic output, just ignore the output
template<typename O>
struct Void:O {
  template<typename T>
  inline static void raw(T) {}
  template<typename I>
  inline static void printItem(I& i) {i.print();}
  inline static void newView() {}
  inline static constexpr bool partialDraw() {return false;}
  template<Roles role,bool io,bool toPrint>
  inline static void fmt(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  inline static constexpr bool isSame(void*) {return false;}
  inline static constexpr Idx top() {return 0;}
  template<typename Nav> inline static constexpr bool posTop(Nav&) {return false;}
  template<typename It,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(It& it,Idx idx=0,Idx top=0) {
    for(auto o:it) o->printItem(O::obj());
  }
  //to detect target menu change when nothing else changed
  inline static void lastDrawn(void*) {}
  constexpr static inline Idx orgX() {return 0;}
  constexpr static inline Idx orgY() {return 0;}
  constexpr static inline Idx width() {return idx_max;}
  constexpr static inline Idx height() {return idx_max;}
  inline static void setCursor(int x,int y) {}
  inline static constexpr int ascent() {return 0;}
};

////////////////////////////////////////////////////////////////////////////////
//basic navigation (no nav state)
template<typename N>
struct Drift:N {
  template<typename Out> inline static void print(Out& out) {}
  inline static Idx pos() {return 0;}
};

////////////////////////////////////////////////////////////////////////////////
// menu items base
template<typename I>
struct Empty:I {
  inline static Idx len() {return 0;}
  template<typename Out,Op=Op::Printing> inline void print(Out&) {}

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
    out.template printItems<I::Type>();
  }

  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printItem(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
    I::obj().print(out);
  }

};
