/* -*- C++ -*- */
#pragma once

#include "base.h"

struct Nil {};

////////////////////////////////////////////////////////////////////////////////
struct Void:Nil {
  using This=Void;
  // template<typename Out,bool toPrint=true>
  // inline static void nl(Out&) {}
  template<typename T,typename Out,bool toPrint=true>
  inline static void print(T o,Out& out,Roles role=Roles::Raw) {out.raw(o,role);}
  template<typename T>
  inline static void raw(T,Roles=Roles::Raw) {}
  inline static void newView() {}
  template<Roles role,bool io,bool toPrint>
  inline static void fmt(Idx=0,bool=false,bool=true,Modes=Modes::Normal) {}
  inline static void clrLine(Idx) {}
  constexpr static inline Idx orgX() {return 0;}
  constexpr static inline Idx orgY() {return 0;}
  constexpr static inline Idx width() {return idx_max;}
  constexpr static inline Idx height() {return idx_max;}
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

  inline static constexpr int maxCharWidth() {return 1;}
  inline static constexpr int maxCharHeight() {return 1;}

  //print loopback by default
  template<typename Nav,typename Out>
  inline static void printOn(Nav& nav,Out& out) {nav._printMenu(nav,out);}
  inline static constexpr bool isSame(void*) {return false;}
  inline static void lastDrawn(void*) {}
};

struct TextMeasure: Void {
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
//imput API
struct None:Nil {
  template<bool onField=false>
  inline static constexpr Cmds cmd() {return Cmds::None;}
};

////////////////////////////////////////////////////////////////
template<typename It,typename Nav,typename Out> struct PrintAgent;
template<typename It,typename Nav>struct NavAgent;

struct Rookie {
  virtual Idx size()=0;
  virtual bool enabled()=0;
  virtual bool canNav()=0;
  virtual bool isMenu()=0;
  virtual bool activate()=0;
  virtual bool parentDraw()=0;
  virtual bool changed()=0;
  virtual void changed(bool)=0;
  virtual void changed(Idx,bool)=0;
};

template<typename It>
struct Agent:public Rookie {
  It& it;
  inline Agent(It& it):it(it) {}
  inline It& agent() {return it;}
  inline It agent() const {return it;}
};
template<typename It,typename Nav>
struct NavAgent:Agent<It> {
  Nav nav;
  inline NavAgent(Agent<It>& ita):Agent<It>(ita),nav(nav) {}
  using Agent<It>::agent;
  virtual void cmd(Nav& nav,Cmds c) {
    switch(c) {
      case Cmds::None: break;
      case Cmds::Enter: agent().enter();break;
      case Cmds::Esc: agent().esc();break;
      case Cmds::Up: agent().up();break;
      case Cmds::Down: agent().down();break;
      case Cmds::Left: agent().left();break;
      case Cmds::Right: agent().right();break;
    }
  }
};
template<typename It,typename Nav,typename Out>
struct PrintAgent:NavAgent<It,Nav> {
  Out& out;
  inline PrintAgent(Agent<It>& ita,Nav& nav,Out& out):NavAgent<It,Nav>(ita,nav),out(out) {}
  inline PrintAgent(NavAgent<It,Nav>& nava,Out& out):NavAgent<It,Nav>(nava),out(out) {}
  using Agent<It>::agent;
  virtual void print(Out& out,Roles role=Roles::Raw,bool toPrint=true) {
    switch(role) {
      case Roles::Raw: toPrint?agent().template print<Roles::Raw,true>(out):agent().template print<Roles::Raw,false>(out);
      case Roles::Panel: toPrint?agent().template print<Roles::Panel,true>(out):agent().template print<Roles::Panel,false>(out);
      case Roles::Menu: toPrint?agent().template print<Roles::Menu,true>(out):agent().template print<Roles::Menu,false>(out);
      case Roles::Title: toPrint?agent().template print<Roles::Title,true>(out):agent().template print<Roles::Title,false>(out);
      case Roles::Body: toPrint?agent().template print<Roles::Body,true>(out):agent().template print<Roles::Body,false>(out);
      case Roles::Item: toPrint?agent().template print<Roles::Item,true>(out):agent().template print<Roles::Item,false>(out);
      case Roles::Index: toPrint?agent().template print<Roles::Index,true>(out):agent().template print<Roles::Index,false>(out);
      case Roles::Cursor: toPrint?agent().template print<Roles::Cursor,true>(out):agent().template print<Roles::Cursor,false>(out);
      case Roles::Name: toPrint?agent().template print<Roles::Name,true>(out):agent().template print<Roles::Name,false>(out);
      case Roles::Mode: toPrint?agent().template print<Roles::Mode,true>(out):agent().template print<Roles::Mode,false>(out);
      case Roles::Value: toPrint?agent().template print<Roles::Value,true>(out):agent().template print<Roles::Value,false>(out);
      case Roles::Unit: toPrint?agent().template print<Roles::Unit,true>(out):agent().template print<Roles::Unit,false>(out);
    }
  }
  virtual void printMenu(bool) {
    trace(MDO<<"PrintAgent::printMenu"<<endl);
    agent().print(out);}
  static inline void printMenu(bool,It& it,Nav&,Out& out,Ref,Idx) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.print(out);}
  template<Roles role=Roles::Raw,bool toPrint=true>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Modes=Modes::Normal) {
    trace(MDO<<"Nav::printItem"<<endl);
    it.template print<Out,role,toPrint>(out);
  }
};

template<typename T>
struct CRTP {
  inline T& item() {return *(T*)this;}
  inline T& operator[](size_t) {return item();}
};

struct Empty:Nil {
  using Base=Nil;
  inline static constexpr Idx size(Ref ref,Idx n=0) {return size();}
  inline static constexpr Idx size() {return 0;}
  template<typename Out,Roles=Roles::Raw,bool=true>
  inline static void print(Out&) {}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Item,OutOp op=OutOp::Printing>
  inline static void printItems(It&,Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    trace(MDO<<"Empty::printItems"<<endl);
  }
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
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {nav.template _cmd<c>();}
  template<typename It,typename Nav,typename Out>
  static inline void printMenu(bool,It& it,Nav&,Out& out) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.print(out);}
  template<typename It,typename Nav,typename Out>
  static inline void printMenu(bool,It& it,Nav&,Out& out,Ref,Idx) {
    trace(MDO<<"Empty::printMenu"<<endl);
    it.print(out);}
  template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
  static inline void printItem(It& it,Out& out,Idx=0,bool=false,bool=true,Modes=Modes::Normal) {
    trace(MDO<<"Nav::printItem"<<endl);
    it.template print<Out,role,toPrint>(out);
  }
  inline static constexpr bool changed() {return false;}
  inline static void changed(bool) {}
  inline static void changed(Idx,bool) {}
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
  inline void printMenu(Out& out) {
    trace(MDO<<"Nav::printMenu(Out)"<<endl);
    out.printOn(*this,out);}
  template<typename Nav,typename Out>
  inline void printMenu(Nav& nav,Out& out) {
    trace(MDO<<"Nav::printMenu(Nav,Out)"<<endl);
    out.printOn(nav,out);}
  template<typename Nav,typename Out>
  inline void _printMenu(Nav& nav,Out& out) {
    trace(MDO<<"Nav::_printMenu"<<endl);
    bool pd=entry.parentDraw(parent());
    entry.template printMenu<decltype(entry),Nav,Out>(pd,entry,nav,out,parent());
  }

  template<typename In> inline bool doInput(In& in) {return doInput(in,*this);}
  template<typename In,typename Nav>
  inline bool doInput(In& in,Nav& nav) {
    Cmds c=in.cmd();
    switch(c) {
      case Cmds::Up:cmd<Cmds::Up,Nav>(nav);break;
      case Cmds::Down:cmd<Cmds::Down,Nav>(nav);break;
      case Cmds::Enter:cmd<Cmds::Enter,Nav>(nav);break;
      case Cmds::Esc:cmd<Cmds::Esc,Nav>(nav);break;
      default:return false;
    }
    return true;
  }
  inline void _up() {
    _trace(
      MDO
        <<"_up() pos():"<<pos()
        <<" < "
        <<" size(parent())-1):"<<size(parent())-1
        <<" size()-1:"<<size()-1
        <<endl;
    );
    if(pos()<size(parent())-1) setPos(pos()+1);}
  inline void _down() {if(pos()>0) setPos(pos()-1);}
  inline void _enter() {
    trace(MDO<<"enter->sending activate"<<endl);
    bool n=entry.canNav(*this,head());
    bool r=entry.activate(*this,head());
    trace(MDO<<"canNav:"<<n<<" activated:"<<r<<endl);
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

  inline void up() {cmd<Cmds::Up,This>(*this);}
  inline void down() {cmd<Cmds::Down,This>(*this);}
  inline void enter() {cmd<Cmds::Enter,This>(*this);}
  inline void esc() {cmd<Cmds::Esc,This>(*this);}

  template<typename Nav> inline void up(Nav& nav) {cmd<Cmds::Up,Nav>(nav);}
  template<typename Nav> inline void down(Nav& nav) {cmd<Cmds::Down,Nav>(nav);}
  template<typename Nav> inline void enter(Nav& nav) {cmd<Cmds::Enter,Nav>(nav);}
  template<typename Nav> inline void esc(Nav& nav) {cmd<Cmds::Esc,Nav>(nav);}
  inline void left() {level--;}
  inline void right() {level++;}

  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav) {entry.template cmd<c,Nav>(nav,((Ref)*this).parent());}

  Idx level=0;
  Modes editMode;
  Idx path[max_depth];
};
