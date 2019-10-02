/* -*- C++ -*- */
#pragma once
#include "base.h"

//a reference to menu item that works also for static structures
struct Ref {
  Idx len;
  Idx* path;
  inline Idx head() const {return path[0];}
  inline Ref tail() const {return {len-1,&path[1]};}
};

struct Nil {};

////////////////////////////////////////////////////////////////////////////////
template<typename N=Nil> struct Drift:N {
  // template<typename It,typename Nav,typename Out>
  // inline static void print(It&,Nav&,Out&,Ref,Idx) {}
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool enabled(Idx) {return true;}
};

////////////////////////////////////////////////////////////////////////////////
template<typename O=Nil> struct Void:O {
  template<Roles r,bool io,typename I,typename Out,typename Nav>
  static inline void fmt(Idx n,Nav& nav,Out& out) {}
  static inline void clrLine(Idx) {}
  // constexpr static inline bool isRange() {return false;}
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline Idx height() {return 0;}
  constexpr static inline Idx top() {return 0;}
  constexpr static inline Idx posX() {return 0;}
  constexpr static inline Idx posY() {return 0;}
  constexpr static inline Idx freeX() {return idx_max;}
  constexpr static inline Idx freeY() {return idx_max;}
  constexpr static inline Idx free() {return idx_max;}
  static inline void useX(Idx ux=1) {}
  static inline void useY(Idx uy=1) {}
};

////////////////////////////////////////////////////////////////////////////////
template<typename I=Nil> struct Empty:I {
  constexpr static inline bool enabled() {return true;}
  inline static void enable(Idx,bool) {}
  inline static Idx size() {return 0;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  static inline void fmt(Idx n,Nav& nav,Out& out) {
    // _trace(MDO<<"<"<<(io?"":"\\")<<r<<">");
    out.template fmt<r,io,It,Out,Nav>(n,nav,out);
  }
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    it.template print<It,Nav,Out>(it,nav,out,ref,n);
  }
};
