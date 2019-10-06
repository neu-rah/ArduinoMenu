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
//provide some static data to static functions
template<typename O> struct Data:O {using O::O;static O obj;};

////////////////////////////////////////////////////////////////////////////////
template<typename N=Nil> struct Drift:N {
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool enabled(Idx) {return true;}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"Drift::cmd:"<<c<<endl);
  }
};

////////////////////////////////////////////////////////////////////////////////
template<typename O=Nil> struct Void:O {
  template<Roles r,bool io,typename I,typename Out,typename Nav>
  static inline void fmt(Idx n,Nav& nav) {}
  static inline void clrLine(Idx) {}
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
  inline static constexpr Idx size(Ref ref) {return 0;}
  inline static constexpr Idx size(Ref ref,Idx n) {return 0;}
  template<Roles r,bool io,typename It,typename Out,typename Nav>
  static inline void fmt(Idx n,Nav& nav) {
    Out::template fmt<r,io,It,Out,Nav>(n,nav);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.template print<Out>();
  }
  template<typename It,typename Nav,typename Out>
  inline static void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    _trace(MDO<<"Empty::printMenu"<<endl);
  }
  template<Cmds c,typename It,typename Nav>
  inline static bool cmd(It& it,Nav& nav,Ref,Idx) {
    _trace(MDO<<"Empty::cmd "<<c<<endl);
    return nav.template _cmd<c,It,Nav>(it,nav);
  }
  template<Cmds c,typename It,typename Nav>
  inline static bool cmd(It& it,Nav& nav) {
    trace(MDO<<"Empty::cmd"<<endl);
    return nav.template _cmd<c,It,Nav>(it,nav);
  }
};
