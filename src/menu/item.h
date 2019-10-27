/* -*- C++ -*- */
#pragma once
// #include <lpp.h>
#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<typename I,ActionHandler act>
struct Action:I {
  using I::I;
  using This=Action<I,act>;
  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav) {return act();}
};

template<const char** text,typename I=Empty<>>
struct StaticText:I {
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {print<Out>();}
};

//wrap an item with static prefix/suffix content
template<typename Of,typename Prefix,typename Suffix=Empty<>>
struct StaticWrap:Of {
  using Of::print;
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void print(It& it,Nav& nav) {
    Prefix::template print<It,Nav,Out,P>(it,nav);
    Of::template print<It,Nav,Out,P>(it,nav);
    Suffix::template print<It,Nav,Out,P>(it,nav);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////
// compile time list
template<typename I,typename... II>
struct StaticData:StaticData<I> {
  using This=StaticData<I,II...>;
  using Head=I;
  using Base=StaticData<I>;
  using Tail=StaticData<II...>;
  using Base::size;
  Tail next;

  inline static constexpr Idx size() {return Tail::size()+1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return
      n?Tail::size(ref,n-1):
        ref.len?
          I::size(ref.tail(),ref.tail().head()):
          size();
  }
  using I::parentDraw;
  inline static constexpr bool parentDraw(Idx n) {
    return n?Tail::parentDraw(n-1):I::parentDraw();
  }
  using I::enabled;
  inline bool enabled(Idx n) {return n?next.enabled(n-1):enabled();}
  inline bool enabled(Ref ref,Idx n) {
    return n?next.enabled(ref,n-1):
      ref.len? enabled(ref.tail(),ref.head()):enabled();
  }

  using I::enable;
  inline void enable(Idx n,bool b) {return n?next.enable(n-1,b):enable(b);}
  inline void enable(Ref ref,Idx n,bool e) {
    if (n) next.enable(ref,n-1,e);
    else if(ref.len) enable(ref.tail(),ref.head(),e);
    else enable(e);
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Idx n,Idx p=0) {
    if (!Out::freeY()) return;
    if (p) next.template printItem<It,Nav,Out,P>(it,nav,n,p-1);
    StaticData<I>::template printItem<I,Nav,Out>(*this,nav,n);
    next.Tail::template printItem<It,Nav,Out>(it,nav,n+1);
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    trace(MDO<<"StaticData<I,II...>::printItems"<<endl);
    Out::posTop(nav);
    if (!Out::freeY()) return;
    Out::clrLine(Out::posY());
    Out::template fmt<P,true,It,Out,Nav>(nav);
    printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
    Out::template fmt<P,false,It,Out,Nav>(nav);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    if (n) next.template printMenu<It,Nav,Out>(it,nav,ref,n-1);
    else if (ref.len)
      I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.tail().head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
  }

  using I::cmd;
  using StaticData<I>::doNav;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (n) {
      return next.Tail::template cmd<c,It,Nav>(it,nav,ref,n-1);
    } else if (ref.len) {
      return I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.tail().head());
    } else return Base::template doNav<c,It,Nav>(it,nav);
  }
  using StaticData<I>::canNav;
  inline static bool canNav(Ref ref,Idx n) {
    if (n) return Tail::canNav(ref,n-1);
    else if (ref.len) return I::canNav(ref.tail(),ref.head());
    else return StaticData<I>::canNav();
  }
};

/////////////////////////////////////////////////////////////////////////////////////////
// compile time list termination
template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  using I::parentDraw;
  inline static constexpr bool parentDraw(Idx n) {
    return I::parentDraw();
  }
  inline static constexpr bool canNav() {return true;}
  inline static constexpr bool canNav(Ref ref,Idx n) {return n?false:canNav();}
  inline static constexpr Idx size() {return 1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return n?1:ref.len?I::size(ref.tail(),ref.tail().head()):I::size()+1;
  }
  using I::enabled;
  inline bool enabled(Idx n) {return n?true:enabled();}

  using I::enable;
  inline void enable(Idx n,bool b) {if(!n) I::enable(b);}


  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Idx n,Idx p=0) {
    if (!Out::freeY()) return;
    trace(MDO<<"printItem #"<<n<<" role:"<<p<<"=>");
    Out::clrLine(Out::posY());
    it.template fmt<Roles::Prompt,true ,It,Out,Nav>(nav,n);
    it.template fmt<Roles::Index, true ,It,Out,Nav>(nav,n);
    it.template fmt<Roles::Index, false,It,Out,Nav>(nav,n);
    it.template fmt<Roles::Cursor,true ,It,Out,Nav>(nav,n);
    it.template fmt<Roles::Cursor,false ,It,Out,Nav>(nav,n);
    reinterpret_cast<I*>(this)->template print<I,Nav,Out,Roles::Prompt>(*reinterpret_cast<I*>(this),nav);
    it.template fmt<Roles::Prompt,false,It,Out,Nav>(nav,n);
  }
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
    Out::posTop(nav);
    it.template fmt<P,true,It,Out,Nav>(nav,0);
    printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
    it.template fmt<P,false,It,Out,Nav>(nav,0);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    assert(!n);
    if (n) return;
    else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
  }
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (n)
      assert(!n);
    if (n||!enabled()) return false;
    if (ref.len) return I::template cmd<c,I,Nav>(*reinterpret_cast<I*>(this),nav,ref,n);
    return doNav<c,It,Nav>(it,nav);
  }
  template<Cmds c,typename It,typename Nav>
  inline bool doNav(It& it,Nav& nav) {
    if(c==Cmds::Enter){
      if(I::template cmd<Cmds::Activate,It,Nav>(it,nav)) {
        if (I::canNav()) {nav.open();return true;}
      } else if (!I::canNav()) {nav.close();return true;}
    }
    return Empty<>::template cmd<c,It,Nav>(it,nav);//will pass cmd back to navigation
  }
};

template<typename T,typename B>
struct StaticMenu:B {
  using This=StaticMenu<T,B>;
  using Title=T;
  using Body=B;
  template<typename Out> inline static void print() {T::template print<Out>();}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {
    Title::template print<Out>();
  }
  using B::cmd;
  template<Cmds c,typename It,typename Nav>
  inline constexpr bool cmd(It& it,Nav& nav) {return true;}

  using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    if (ref.len==1&&B::parentDraw(n)) Out::template printParent<It,Nav,Out>(it,nav);
    else if (ref.len) B::template printMenu<It,Nav,Out>(it,nav,ref,n);
    else Out::template printMenu<This,Nav,Out>(*this,nav);
  }
};
