/* -*- C++ -*- */
#pragma once

#include "base.h"

struct Nil {};

////////////////////////////////////////////////////////////////////////////////
struct Void:Nil {
  using This=Void;
  template<typename Out,bool toPrint=true>
  inline static void nl(Out&) {}
  template<typename T,typename Out,bool toPrint=true>
  inline static void raw(T,Out&,Roles=Roles::Raw) {}
  // template<typename T>
  // inline static void raw<T,This>(T o,Roles role=Roles::Raw) {raw<T,This>(o,*this);}
  inline static void newView() {}
  template<Roles role,bool io>
  inline static void fmt(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  inline static void clrLine(Idx) {}
  inline static void posY(Idx) {}
  constexpr static inline Idx posY() {return 0;}
  constexpr static inline Idx freeY() {return idx_max;}
  inline static void setClip(int x,int y,int w,int h) {}
  inline static void clipOff() {}
  inline static constexpr Idx top() {return 0;}
  template<typename Nav> inline static constexpr bool posTop(Nav&) {return false;}
  inline static constexpr int ascent() {return 0;}
  inline static void setCursor(int x,int y) {}

  inline static constexpr bool partialDraw() {return false;}

  template<bool io> inline static void fmtRaw(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtPanel(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtMenu(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtTitle(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtBody(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtItem(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtIndex(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtCursor(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtName(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtMode(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtValue(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  template<bool io> inline static void fmtUnit(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}

  inline static constexpr int maxCharWidth() {return 1;}
  inline static constexpr int maxCharHeight() {return 1;}

  //print loopback by default
  template<typename Nav,typename Out>
  inline static void printOn(Nav& nav,Out& out) {nav._printMenu(out);}
};

struct TextMeasure:public Void {
  template<typename T>
  static inline Idx measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  inline static constexpr int maxCharWidth() {return 1;}
  inline static constexpr int maxCharHeight() {return 1;}
  int textWidth(const char*s) const {return measure(s);}
  protected:
    #ifndef ARDUINO
    static inline Idx _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline Idx _str(T i){return std::string(std::to_string(i)).length();}
    #endif
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
  template<typename Out,Roles=Roles::Raw,bool=true>
  inline static void print(Out&) {}
  // template<typename It,typename Nav,typename Out>
  // inline void printMenu(bool,It&,Nav& nav,Out& out,Ref ref,Idx n) {}
  template<typename Nav,typename Out>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {}
  inline static void enable(bool) {}
  inline static void enable(bool,Idx) {}
  inline static void enable(bool,Ref,Idx) {}
  inline static constexpr bool enabled() {return true;}
  inline static constexpr bool enabled(Idx) {return enabled();}
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
  template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Modes=Modes::Normal) {
    it.template print<Out,role,toPrint>(out);
  }
  inline static constexpr bool changed() {return false;}
  inline static void changed(bool) {}
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
    // assert(level>0);
    if(level>0) {
      path[level--]=0;
      setMode(Modes::Normal);
    }
  }
  inline operator Ref() const {return Ref{(Idx)(level+1),path};}
  inline Ref parent() const {return operator Ref().parent();}
  inline Idx size() const {return entry.size((Ref)*this);}
  inline Idx size(Ref ref) const {return entry.size(ref);}
  inline void enable(bool e) {entry.enable(e,*this);}
  inline void enable(bool e,Ref ref) {entry.enable(e,ref);}
  inline bool enabled() const {return enabled(*this);}
  inline bool enabled(Ref ref) const {return entry.enabled(ref);}
  template<typename Out>
  inline void printMenu(Out& out) {out.printOn(*this,out);}
  template<typename Out>
  inline void _printMenu(Out& out) {
    bool pd=entry.parentDraw(parent());
    entry.template printMenu<Root,This,Out>(pd,entry,*this,out,parent());
  }

  template<typename In>
  inline bool doInput(In& in) {return in.cmd(*this);}
  inline void _up() {
    if(pos()<size(parent())-1) setPos(pos()+1);}
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
