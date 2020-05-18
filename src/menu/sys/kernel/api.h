/* -*- C++ -*- */
#pragma once

#include "base.h"
#include "path.h"

namespace Menu {

  template<typename O>
  struct IdMatch {
    using Result=typename O::Result;
    bool match=false;
    Result res;
    IdMatch() {}
    IdMatch(O o):match(true),res(o){}
    operator Result() {assert(match);return res;}
  };

  /////////////////////////////////////////////////////////
  // APICall Call agents
  struct APICall {
    template<typename CRTP>
    struct Call {
      inline constexpr const char* named() {return CRTP::name;}
      const CRTP& obj() const {return *(CRTP*)this;}
      template<typename At>
      inline APIRes step(At& at,PathRef ref,Idx level) const {
        return walkPath(at,ref,level+1);
      }
      template<typename At>
      inline APIRes walkPath(At& at,PathRef ref,Idx level=0) const {
        trace(/*if(!ref)*/ MDO<<"APICall::walkPath "<<named()<<" "<<ref<<endl);
        if (ref.len==0) return APIRes(obj().call(at,level));//self
        return ref?
          at.walkPath(obj(),ref.tail(),level,ref.head())://continue
          APIRes(obj().callItem(at,level,ref.head()));//child
      }
      template<typename At>
      inline APIRes walkId(At& at,Idx id,Idx level=0) const {return at.walkId(obj(),id,level);}
      template<typename At,Idx id>
      inline APIRes walkId(At& at,Idx level=0) const {
        return at.template walkId<CRTP,id>(obj(),level);
      }
    };

    template<Cmd c,typename Nav,Idx id=0>
    struct Cmd:Call<Cmd<c,Nav>> {
      using This=Cmd<c,Nav,id>;
      using Base=Call<Cmd<c,Nav>>;
      using Result=bool;
      Nav& nav;
      Idx cmdAux;
      inline Cmd(Nav& nav,Idx n=0):nav(nav),cmdAux(n) {}
      template<typename At>
      inline APIRes call(At& at,Idx level) const {
        trace(MDO<<"APICall::Cmd::call "<<c<<endl);
        return at.template cmd<c,Nav>(nav,level,cmdAux);
      }
      template<typename At>
      inline APIRes callItem(At& at,Idx level,Idx n) const {
        trace(MDO<<"APICall::Cmd::call "<<c<<" #"<<n<<endl);
        return at.template cmdItem<c,Nav>(nav,level,cmdAux,n);
      }
      static constexpr const char* name="Cmd";
    };
    template<typename Nav>
    struct Activate:Call<Activate<Nav>> {
      Nav& nav;
      using Result=void;
      Activate(Nav& nav):nav(nav) {}
      template<typename At>
      inline APIRes call(At& at,Idx level) const {
        if(at.enabled()) at.activate(nav,level);
        return APIRes();
      }
      template<typename At>
      inline APIRes callItem(At& at,Idx level,Idx n) const {
        if(at.enabled()) at.activateItem(nav,level,n);
        return APIRes();
      }
      static constexpr const char* name="Activate";
    };
    template<bool b>
    struct Enable:Call<Enable<b>> {
      using Result=void;
      template<typename At>
      inline static APIRes call(At& at,Idx level) {at.enable(b);return APIRes();}
      template<typename At>
      inline static APIRes callItem(At& at,Idx level,Idx n) {at.enableItem(b,n);return APIRes();}
      static constexpr const char* name="Enable";
    };
    struct Enabled:Call<Enabled> {
      using Result=bool;
      template<typename At>
      inline static APIRes call(At& at,Idx level) {return at.enabled();}
      template<typename At>
      inline static APIRes callItem(At& at,Idx level,Idx n) {return at.enabledItem(n);}
      static constexpr const char* name="Enabled";
    };
    struct Size:Call<Size> {
      using Result=size_t;
      template<typename At>
      inline static APIRes call(At& at,Idx level) {return at.size();}
      template<typename At>
      inline static APIRes callItem(At& at,Idx level,Idx n) {return at.sizeItem(n);}
      static constexpr const char* name="Size";
    };
    template<typename Nav,typename Out,Op op=Op::Printing>
    struct PrintMenu:Call<PrintMenu<Nav,Out,op>> {
      using Result=void;
      Nav& nav;
      Out& out;
      inline PrintMenu(Nav& nav,Out& out):nav(nav),out(out) {
        trace(MDO<<"APICall::PrintMenu"<<endl);
      }
      template<typename At>
      inline APIRes call(At& at,Idx level) const {
        trace(MDO<<"APICall::PrintMenu::call=> level:"<<level<<endl);
        at.template printMenu<Nav,Out,op>(nav,out,level);
        return APIRes();
      }
      template<typename At>
      inline APIRes callItem(At& at,Idx level,Idx n) const {
        trace(MDO<<"APICall::PrintMenu::call=> level:"<<level<<" idx:"<<n<<endl);
        at.template printMenuItem<Nav,Out,op>(nav,out,level,n);
        return APIRes();
      }
      static constexpr const char* name="PrintMenu";
    };
  };

  ////////////////////////////////////////////////////////////////////////////////
  //basic input, just be quiet
  template<typename In>
  struct Quiet:In {
    template<typename Nav>
    inline static constexpr bool parseKey(Nav&) {return false;}
    template<typename Nav>
    inline static constexpr bool parseCmd(Nav&,Key,bool=false) {return false;}

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
  };

  ////////////////////////////////////////////////////////////////////////////////
  //basic navigation (no nav state)
  template<typename N>
  struct Drift:N {
    template<typename Out> inline static void print(Out& out) {}
    inline static Idx pos() {return 0;}
    inline static bool selected(Idx) {return false;}
    inline static bool selected(Idx,Idx) {return false;}
    inline Mode mode() const {return Mode::Normal;}
    // template<Cmd c> inline static bool _cmd() {return false;}
    template<Cmd c> inline static bool cmd() {return false;}
    inline static void up(Cmd) {}
    inline static void down(Cmd) {}
    inline static void enter(Cmd) {}
    inline static void esc(Cmd) {}
    template<typename In>
    inline bool doInput(In& in) {return in.cmd(N::obj());}
    inline static void open() {}
    inline static void close() {}
};

  ////////////////////////////////////////////////////////////////////////////////
  // menu items base
  template<typename I>
  struct Empty:I {
    using Base=I;
    using Base::Base;
    template<typename A,Idx i> APIRes walkId(const A& api,Idx level) const {return api.call(I::obj(),level);}
    template<typename A> APIRes walkId(const A& api,Idx level,Idx i) const {return api.call(I::obj(),level);}

    template<Idx tag>
    static inline constexpr bool id() {return false;}
    static inline constexpr bool id(Idx) {return false;}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,Idx level) {
      trace(MDO <<"Empty::printMenu"<<endl);
      if (op==Op::Printing) I::obj().print(nav,out,level,false);
    }

    inline static constexpr bool canNav() {return false;}
    inline static constexpr bool canNav(Idx) {return false;}
    inline constexpr size_t size() const {return 0;}
    inline constexpr size_t count() const {return 0;}
    template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
    inline static void print(Nav& nav,Out& out,Idx level,bool selected) {}
    template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
    inline static void printItem(Nav& nav,Out& out,Idx level,Idx) {}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline static void printItems(Nav& nav,Out& out,Idx level,bool fullPrint,Idx idx=0,Idx top=0) {}
    inline static dbg_constexpr bool enabled() {
      trace(MDO<<"Empty::enabled()"<<endl);
      return true;}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printTitle(Nav& nav,Out& out,Idx level,bool fullPrint) {
      trace(MDO<<"Empty::printTitle"<<endl);
      if(fullPrint||I::obj().changed()) I::obj().template print<Nav,Out,Op::Printing>(nav,out,level,false);
    }
    inline static void enable(bool b) {
      trace(MDO<<"Empty::enable("<<b<<")"<<endl);
    }
    inline static constexpr bool changed() {return true;}
    inline static void changed(bool o) {}
    inline static void changed(Idx,bool o) {}
    template<typename Nav>
    inline void activate(Nav& nav,Idx) {
      trace(MDO<<"Empty::activate"<<endl);
      if(Base::obj().enabled()) nav.close();}

    template<typename Nav>
    inline static dbg_constexpr bool action(Nav&,Idx) {
      trace(MDO<<"Empty::action"<<endl);
      return true;}

    template<typename T> inline static T getValue(Idx n) {
      assert(false);
      return (T)0;
    }
    template<typename T> inline static void setValue(T&) {}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx level,Idx aux) {
      trace(MDO<<"Empty::cmd "<<c<<" aux:"<<aux<<" level:"<<level<<endl);
      return false;
    }

    template<Cmd c,typename Nav>
    inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
      trace(MDO<<"Empty::cmdItem "<<c<<" aux:"<<aux<<" level:"<<level<<" #"<<n<<endl);
      return false;
    }

    template<typename A>
    APIRes walkPath(const A& api,PathRef ref,Idx level) {
      trace(MDO<<"default stepper"<<endl);
      return api.step(Base::obj(),ref,level);}
    template<typename A>
    APIRes walkPath(const A& api,PathRef ref,Idx level,Idx n) {
      trace(MDO<<"default stepper n:"<<n<<endl);
      return api.step(Base::obj(),ref,level);}
    template<typename Nav>
    inline void activateItem(Nav& nav,Idx level,Idx) {
      trace(MDO<<"Empty::activateItem"<<endl);
      Base::obj().activate(nav,level);}
    inline void enableItem(bool b,Idx) {
      trace(MDO<<"Empty::enable"<<endl);
      Base::obj().enable(b);}
    inline bool enabledItem(Idx n) const {
      trace(MDO<<"Empty::enabled"<<endl);
      return Base::obj().enabled();}
    inline size_t sizeItem(Idx) const {
      trace(MDO<<"Empty::size"<<endl);
      return Base::obj().size();}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenuItem(Nav& nav,Out& out,Idx level,Idx) {
      trace(MDO<<"Empty::printMenuItem"<<endl);
      Base::obj().template printMenu<Nav,Out,op>(nav,out,level);
    }

  };

  /**
  * The Item class encapsulates a composition to be a stratic menu item.
  */
  template<Expr... I>
  struct Item:Chain<I...,Empty>::template To<Obj<Item<I...>>> {
    using Base=typename Chain<I...,Empty>::template To<Obj<Item<I...>>>;
    using This=Item<I...>;
    using Base::Base;
  };

};
