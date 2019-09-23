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
  template<typename Nav> static inline bool up(Nav&) {return false;}
  template<typename Nav> static inline bool down(Nav&) {return false;}
  template<typename Nav> static inline bool enter(Nav&) {return false;}
  template<typename Nav> static inline bool esc(Nav&) {return false;}

  //system default actions
  static inline bool _up() {return false;}
  static inline bool _down() {return false;}
  static inline bool _enter() {return false;}
  static inline bool _esc() {return false;}

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
  // constexpr static inline bool inRange(idx_t) {return true;}
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
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtPanel (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtMenu  (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtTitle (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtBody  (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtItem  (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtIndex (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtCursor(idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtName  (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtMode  (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtValue (idx_t,const Nav&,const Out&) {}
  template<bool io,typename I,typename Out,typename Nav> static inline void fmtUnit  (idx_t,const Nav&,const Out&) {}
};

////////////////////////////////////////////////////////////////////////////////
//menu structure terminal
template<typename I>
struct Empty:I {
  template<typename Nav> inline static bool up(idx_t,Nav& nav) {return nav._up();}
  template<typename Nav> inline static bool down(idx_t,Nav& nav) {return nav._down();}
  template<typename Nav> inline static bool enter(idx_t,Nav& nav) {
    _trace(MDO<<"Empty::enter"<<endl);
    return nav._enter();}
  template<typename Nav> inline static bool esc(idx_t,Nav& nav) {return nav._esc();}
  /// is this item enabled?
  constexpr static inline bool enabled() {return true;}
  /// get enabled status of collection indexed item
  constexpr static inline bool enabled(idx_t) {return true;}
  /// set enabled status of indexed collection member
  inline static void enable(idx_t,bool) {}
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
  inline static void fmt(idx_t n,const Nav& nav,Out& out) {Out::template fmt<r,io,It,Out,Nav>(n,nav,out);}
};
