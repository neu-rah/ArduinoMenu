/* -*- C++ -*- */
#pragma once

#include "base.h"

class IItem;
class INav;
class IOut;

struct Nil {};

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
  inline static constexpr bool isSame(void*) {return false;}
  inline static constexpr Idx top() {return 0;}
  inline static constexpr Idx posY() {return 0;}
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
  inline static void clrLine(Idx) {}

  template<bool io,bool toPrint> inline static void fmtRaw(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtPanel(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtMenu(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtTitle(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtBody(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtItem(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtIndex(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtCursor(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtName(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtMode(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtValue(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io,bool toPrint> inline static void fmtUnit(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}

  template<Roles r,bool io,bool toPrint=true>
  inline void fmt(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    switch(r) {
      case Roles::Panel:  O::obj().template fmtPanel <io,toPrint>(n,s,e,m);break;
      case Roles::Menu:   O::obj().template fmtMenu  <io,toPrint>(n,s,e,m);break;
      case Roles::Title:  O::obj().template fmtTitle <io,toPrint>(n,s,e,m);break;
      case Roles::Body:   O::obj().template fmtBody  <io,toPrint>(n,s,e,m);break;
      case Roles::Item:   O::obj().template fmtItem  <io,toPrint>(n,s,e,m);break;
      case Roles::Index:  O::obj().template fmtIndex <io,toPrint>(n,s,e,m);break;
      case Roles::Cursor: O::obj().template fmtCursor<io,toPrint>(n,s,e,m);break;
      case Roles::Name:   O::obj().template fmtName  <io,toPrint>(n,s,e,m);break;
      case Roles::Mode:   O::obj().template fmtMode  <io,toPrint>(n,s,e,m);break;
      case Roles::Value:  O::obj().template fmtValue <io,toPrint>(n,s,e,m);break;
      case Roles::Unit:   O::obj().template fmtUnit  <io,toPrint>(n,s,e,m);break;
      default:break;
    }
  }
};

////////////////////////////////////////////////////////////////////////////////
//basic navigation (no nav state)
template<typename N>
struct Drift:N {
  template<typename Out> inline static void print(Out& out) {}
  inline static Idx pos() {return 0;}
  inline static bool selected(Idx) {return false;}
  inline Modes mode() const {return Modes::Normal;}
  inline static void cmd(Cmd) {}
};

////////////////////////////////////////////////////////////////////////////////
// menu items base
template<typename I>
struct Empty:I {
  inline static constexpr size_t size(PathRef=self,Idx=0) {return 0;}

  inline static constexpr bool parentPrint(PathRef=self) {return true;}

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline static void print(Nav& nav,Out& out,PathRef ref=self) {}

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline static void printItems(Nav& nav,Out& out,Idx=0,Idx=0,PathRef ref=self) {}

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline static void printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref,Idx n) {}

  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef=self) {print<Nav,Out,op>(nav,out);}

  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef,Idx) {print<Nav,Out,op>(nav,out);}

  inline static constexpr bool enabled(PathRef=self) {return true;}
  inline static void enable(bool,PathRef=self) {}

  inline static constexpr bool changed() {return false;}
  inline static void changed(bool o) {}

  // template<Cmds c,typename Nav>
  // inline void cmd(Nav& nav,PathRef=self) {nav.template _cmd<c>();}
  //
  // template<Cmds c,typename Nav>
  // inline void cmd(Nav& nav,Ref ref,Idx n) {nav.template _cmd<c>();}
};
