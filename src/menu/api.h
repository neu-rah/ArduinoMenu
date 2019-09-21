/* -*- C++ -*- */
#pragma once

#include "base.h"


//provide some static data to static functions
template<typename O> struct Data:O {using O::O;static O data;};
template<typename O> O Data<O>::data;


////////////////////////////////////////////////////////////////////////////////
//navigation terminal
template<typename N>
struct Drift:N {
  static inline bool up() {return false;}
  static inline bool down() {return false;}
  static inline bool enter() {return false;}
  static inline bool esc() {return false;}
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline idx_t pos() {return 0;}
  constexpr static inline idx_t size() {return 0;}
};

////////////////////////////////////////////////////////////////////////////////
//output terminal
template<typename O>
struct Void:O {
  template<typename T> inline static void raw(T&) {}
  inline static void nl() {}
  template<typename Out>
  static inline void clrLine(idx_t n) {}
  constexpr static inline bool isRange() {return false;}
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline idx_t height() {return 0;}
  constexpr static inline idx_t top() {return 0;}
  // static inline void setTop(idx_t) {}
  constexpr static inline idx_t posX() {return 0;}
  constexpr static inline idx_t posY() {return 0;}
  constexpr static inline idx_t freeX() {return idx_max;}
  constexpr static inline idx_t freeY() {return idx_max;}
  constexpr static inline idx_t free() {return idx_max;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtPanel (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtMenu  (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtTitle (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtBody  (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtItem  (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtIndex (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtCursor(idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtName  (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtMode  (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtValue (idx_t,const Nav&) {}
  template<bool io,typename Out,typename Nav,typename I> static inline void fmtUnit  (idx_t,const Nav&) {}
};

////////////////////////////////////////////////////////////////////////////////
//menu structure terminal
template<typename I>
struct Empty:I {
  inline static idx_t size() {return 0;}
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printTo() {}//print full item
  template<typename O,Roles P=Roles::Raw,typename N=Drift<>>
  inline static void printItem(size_t i) {}//print a sub-item by index
  template<typename It,typename Out,typename Nav=Drift<>>
  inline static void printMenu(Nav& nav) {
    It::template printTo<Out,Roles::Panel,Nav>();
    It::template printMenu<Out,Nav>(nav);
  }//print full menu
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  inline static void fmt(idx_t n,const Nav& nav) {Out::template fmt<r,io,It,Out,Nav>(n,nav);}
};
