/* -*- C++ -*- */
#pragma once

#include "base.h"
#include "path.h"

namespace Menu {
  struct Nil {};

  ////////////////////////////////////////////////////////////////////////////////
  //basic input, just be quiet
  template<typename In>
  struct Quiet:In {
    template<typename Nav> inline static bool cmd(Nav& nav) {return false;}
  };

  ////////////////////////////////////////////////////////////////////////////////
  //basic output, just ignore the output
  template<typename O>
  struct Void:O {
    template<typename T,bool toPrint=true>
    inline static void raw(T) {}
    inline static void newView() {}
    inline static constexpr bool partialDraw() {return false;}
    inline static constexpr bool isSame(void*) {return false;}
    inline bool fullDraw() {return !O::obj().partialDraw();}
    inline static constexpr Idx top() {return 0;}
    inline static constexpr Idx posY() {return 0;}
    inline static constexpr Idx posX() {return 0;}
    inline static constexpr Idx freeY() {return idx_max;}
    template<typename Nav> inline static constexpr bool posTop(Nav&) {return false;}
    // template<typename It,typename Nav,Op op=Op::Printing,bool toPrint=true>
    // inline static void printTitle(It& it,Nav& nav) {}
    // template<typename It,Op op=Op::Printing,Tag role=Tag::Raw>
    // inline void printItems(It& it,Idx idx=0,Idx top=0) {
    //   for(auto o:it) o->printItem(O::obj());
    // }
    //to detect target menu change when nothing else changed
    inline static void lastDrawn(void*) {}
    constexpr static inline Idx orgX() {return 0;}
    constexpr static inline Idx orgY() {return 0;}
    constexpr static inline Idx width() {return idx_max;}
    constexpr static inline Idx height() {return idx_max;}
    inline static void setCursor(int x,int y) {}
    inline static constexpr int ascent() {return 0;}
    template<bool toPrint=true>
    inline static void clrLine(Idx) {}

    //formating api
    template<bool io,bool toPrint> inline static void fmtRaw(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtPanel(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtMenu(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtTitle(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtBody(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtItem(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtIndex(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtCursor(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtName(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtMode(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtValue(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}
    template<bool io,bool toPrint> inline static void fmtUnit(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {}

    template<bool io> inline static void fmtRaw(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtRaw<io,true>(n,s,e,m);
      else fmtRaw<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtPanel(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtPanel<io,true>(n,s,e,m);
      else fmtPanel<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtMenu(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtMenu<io,true>(n,s,e,m);
      else fmtMenu<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtTitle(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtTitle<io,true>(n,s,e,m);
      else fmtTitle<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtBody(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtBody<io,true>(n,s,e,m);
      else fmtBody<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtItem(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtItem<io,true>(n,s,e,m);
      else fmtItem<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtIndex(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtIndex<io,true>(n,s,e,m);
      else fmtIndex<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtCursor(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtCursor<io,true>(n,s,e,m);
      else fmtCursor<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtName(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtName<io,true>(n,s,e,m);
      else fmtName<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtMode(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtMode<io,true>(n,s,e,m);
      else fmtMode<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtValue(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtValue<io,true>(n,s,e,m);
      else fmtValue<io,false>(n,s,e,m);
    }
    template<bool io> inline static void fmtUnit(bool toPrint,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(toPrint) fmtUnit<io,true>(n,s,e,m);
      else fmtUnit<io,false>(n,s,e,m);
    }

    //format multiplex
    template<Tag r,bool io,bool toPrint=true>
    inline void fmt(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      switch(r) {
        case Tag::Panel:  O::obj().template fmtPanel <io,toPrint>(n,s,e,m);break;
        case Tag::Menu:   O::obj().template fmtMenu  <io,toPrint>(n,s,e,m);break;
        case Tag::Title:  O::obj().template fmtTitle <io,toPrint>(n,s,e,m);break;
        case Tag::Body:   O::obj().template fmtBody  <io,toPrint>(n,s,e,m);break;
        case Tag::Item:   O::obj().template fmtItem  <io,toPrint>(n,s,e,m);break;
        case Tag::Index:  O::obj().template fmtIndex <io,toPrint>(n,s,e,m);break;
        case Tag::Cursor: O::obj().template fmtCursor<io,toPrint>(n,s,e,m);break;
        case Tag::Name:   O::obj().template fmtName  <io,toPrint>(n,s,e,m);break;
        case Tag::Mode:   O::obj().template fmtMode  <io,toPrint>(n,s,e,m);break;
        case Tag::Value:  O::obj().template fmtValue <io,toPrint>(n,s,e,m);break;
        case Tag::Unit:   O::obj().template fmtUnit  <io,toPrint>(n,s,e,m);break;
        default:break;
      }
    }
    //format multiplex
    // template<Tag r,bool io>
    // inline void fmt(bool toPrint=true,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
    //   if(toPrint) fmt<r,io,true>(n,s,e,m);
    //   else fmt<r,io,false>(n,s,e,m);
    // }
  };

  ////////////////////////////////////////////////////////////////////////////////
  //basic navigation (no nav state)
  template<typename N>
  struct Drift:N {
    template<typename Out> inline static void print(Out& out) {}
    inline static Idx pos() {return 0;}
    inline static bool selected(Idx) {return false;}
    inline Mode mode() const {return Mode::Normal;}
    template<Cmd c> inline static bool _cmd() {return false;}
    template<Cmd c> inline static bool cmd() {return false;}
    inline static void up(Cmd) {}
    inline static void down(Cmd) {}
    inline static void enter(Cmd) {}
    inline static void esc(Cmd) {}
    template<typename In>
    inline bool doInput(In& in) {return in.cmd(N::obj());}
  };

  ////////////////////////////////////////////////////////////////////////////////
  // menu items base
  template<typename I>
  struct Empty:I {

    static inline constexpr bool id(Idx tag) {return false;}

    inline static constexpr bool canNav(PathRef=self,Idx=0) {return false;}

    inline static constexpr size_t size(PathRef=self,Idx=0) {return 0;}

    inline static constexpr bool parentPrint(PathRef=self,Idx=0) {return true;}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline static void printItems(Nav& nav,Out& out,bool fullPrint,Idx=0,Idx=0,PathRef ref=self) {
      trace(MDO<<"Empty::printItems "<<ref<<endl);
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline static void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx,Idx top,PathRef ref,Idx n) {
      trace(MDO<<"Empty::printItems "<<ref<<":"<<n<<endl);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Idx i>
    inline static void printMenu(Nav& nav,Out& out) {}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline static void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"Empty::printMenu "<<ref<<":"<<n<<" printing parent"<<endl);
    }

    inline static constexpr bool enabled(PathRef=self,Idx=0) {return true;}
    inline static void enable(bool,PathRef=self,Idx=0) {}

    inline static constexpr bool changed() {return true;}
    inline static void changed(bool o) {}
    inline static void changed(Idx,bool o) {}
    // inline static void changed(Idx i,bool o,PathRef ref,Idx n) {}

    inline static constexpr bool activate(PathRef=self,Idx=0) {return false;}

    template<typename Nav> inline void up(Nav& nav) {nav._up();}
    template<typename Nav> inline void down(Nav& nav) {nav._down();}
    template<typename Nav> inline void enter(Nav& nav) {nav._enter();}
    template<typename Nav> inline void esc(Nav& nav) {nav._esc();}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,PathRef=self,Idx=0) {return nav.template _cmd<c>();}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline static void printTitle(Nav& nav,Out& out,bool) {}

  };
};
