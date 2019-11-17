/* -*- C++ -*- */
#pragma once

#include "base.h"

struct Nil {};

////////////////////////////////////////////////////////////////////////////////
struct Void:Nil {
  inline static void nl() {}
  template<typename T> inline static void raw(T o) {}
  inline static void newView() {}
  template<Roles role,bool io>
  inline static void fmt(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  static inline void clrLine(Idx) {}
  static inline void posY(Idx) {}
  constexpr static inline Idx posY() {return 0;}
  constexpr static inline Idx freeY() {return idx_max;}
};

////////////////////////////////////////////////////////////////////////////////
struct None:Nil {
  template<typename Nav,bool invY=false>
  inline static void cmd(Nav&) {}
};

////////////////////////////////////////////////////////////////
struct Empty:Nil {
  using Base=Nil;
  inline static constexpr Idx size(Ref ref,Idx n) {return size();}
  inline static constexpr Idx size() {return 0;}
  template<typename Out,Roles=Roles::Raw>
  inline static void print(Out&) {}
  // template<typename It,typename Nav,typename Out>
  // inline void printMenu(bool,It&,Nav& nav,Out& out,Ref ref,Idx n) {}
  template<typename Nav,typename Out>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {}
  inline static void enable(bool) {}
  inline static void enable(bool e,Ref ref,Idx n) {}
  inline static constexpr bool enabled() {return true;}
  inline static constexpr bool enabled(Ref,Idx=0) {return enabled();}
  inline static constexpr bool canNav() {return false;}
  inline static constexpr bool canNav(Ref,Idx=0) {return canNav();}
  inline static constexpr bool isMenu() {return false;}
  inline static constexpr bool isMenu(Ref,Idx=0) {return isMenu();}
  inline static constexpr bool activate() {return true;}
  inline static constexpr bool activate(Ref,Idx=0) {return activate();}
  inline static constexpr bool parentDraw(Idx=0) {return true;}
  inline static constexpr bool parentDraw(Ref,Idx) {return parentDraw();}
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav) {nav.template _cmd<c>();}
  // template<Cmds c,typename Nav>
  // inline void cmd(Nav& nav,Ref ref) {nav.template _cmd<c>();}
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {nav.template _cmd<c>();}
  //this functions would benefit from CRTP-----------------
  //using a reference parameter instead
  template<typename It,typename Nav,typename Out>
  static inline void printMenu(bool,It& it,Nav&,Out& out) {it.print(out);}
  template<typename It,typename Nav,typename Out>
  static inline void printMenu(bool,It& it,Nav&,Out& out,Ref,Idx) {it.print(out);}
  template<typename It,typename Out,Roles=Roles::Raw>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Modes=Modes::Normal) {
    trace(MDO<<"Empty::printItem"<<endl);
    it.print(out);
  }
};

///////////////////////////////////////////////////////////////////////////////
template<typename R,R& entry,Idx md>
struct Nav:Nil {
  using This=Nav<R,entry,md>;
  using Root=R;
  static constexpr Idx max_depth=md;
  inline Modes mode() const {
    return editMode;}
  inline void setMode(Modes m) {editMode=m;}
  inline Idx head() const {return path[0];}
  inline Idx pos() const {return path[level];}
  inline bool selected(Idx n) const {return n==pos();}
  inline void setPos(Idx n) {path[level]=n;}
  inline void open() {
    assert(level<md-1);
    if(!entry.isMenu(*this,head())) setMode(Modes::Edit);
    path[++level]=0;
  }
  inline void close() {
    assert(level>0);
    path[level--]=0;
    setMode(Modes::Normal);
  }
  inline operator Ref() const {return Ref{level+1,path};}
  inline Ref parent() const {return operator Ref().parent();}
  inline Idx size() const {entry.size((Ref)*this);}
  inline Idx size(Ref ref) const {entry.size(ref);}
  inline void enable(bool e) {entry.enable(e,*this);}
  inline void enable(bool e,Ref ref) {entry.enable(e,ref);}
  inline bool enabled() const {enabled(*this);}
  inline bool enabled(Ref ref) const {entry.enabled(ref);}
  template<typename Out>
  inline void printMenu(Out& out) {
    bool pd=entry.parentDraw(parent());
    entry.template printMenu<Root,This,Out>(pd,entry,*this,out,parent());
  }

  template<typename In>
  inline bool doInput(In& in) {return in.cmd(*this);}
  inline void _up() {if(pos()<size(parent())-1) setPos(pos()+1);}
  inline void _down() {if(pos()>0) setPos(pos()-1);}
  inline void _enter() {
    bool n=entry.canNav(*this,head());
    bool r=entry.activate(*this,head());
    if (!(n^r)) n?open():close();
  }
  inline void _esc() {close();}

  template<Cmds c>
  inline void _cmd() {
    switch(c) {
      case Cmds::Up:_up();break;
      case Cmds::Down:_down();break;
      case Cmds::Enter:_enter();break;
      case Cmds::Esc:_esc();break;
      default:break;
    }
  }

  inline void up() {cmd<Cmds::Up>();}
  inline void down() {cmd<Cmds::Down>();}
  inline void enter() {cmd<Cmds::Enter>();}
  inline void esc() {cmd<Cmds::Esc>();}

  template<Cmds c>
  inline void cmd() {entry.template cmd<c,This>(*this,((Ref)*this).parent());}

  Idx level=0;
  Modes editMode;
  Idx path[max_depth];
};
