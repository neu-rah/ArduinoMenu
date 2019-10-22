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
  using I::size;
  template<typename Out> inline static void print() {Out::raw(text[0]);}
  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline static void print(It& it,Nav& nav) {print<Out>();}
};

// enum FName {StaticPrint};
// template<FName f>
// using FuncId=lambda::StaticValue<FName,f>;

template<typename T>
struct Value {
  inline Value(T t):v(t){}
  T v;
  template<typename... OO>
  inline T value(OO... oo) const {return v;}
  using Type=T;
};

template<typename R,typename F,typename... OO>
struct Func:Value<F> {
  using Value<F>::Value;
  inline R operator()(OO... oo) const {return f(oo...);}
  using Res=R;
};

template<typename I,typename... II>
struct StaticData:StaticData<I> {
  using This=StaticData<I,II...>;
  using Head=I;
  using Base=StaticData<I>;
  using Tail=StaticData<II...>;
  using Base::size;
  Tail next;

  // template<typename n>
  // using Item=typename lpp::Index<lpp::List<lpp::As<I>,lpp::As<II>...>,n>::App::Type;
  // template<Idx n> inline Item<lpp::N<n>> item() {return n?next.template item<n-1>():*this;};

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
    if (p) next.template printItem<It,Nav,Out,P>(it,nav,n,p-1);
    if (!Out::freeY()) return;
    StaticData<I>::template printItem<I,Nav,Out>(*this,nav,n);
    next.Tail::template printItem<It,Nav,Out>(it,nav,n+1);
  }

  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItems(It& it,Nav& nav) {
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
    else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
  }

  using I::cmd;
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (n) {
      return next.Tail::template cmd<c,It,Nav>(it,nav,ref,n-1);
    } else if (ref.len) {
      return reinterpret_cast<I*>(this)->I::template cmd<c,It,Nav>(it,nav,ref.tail(),nav.pos());
    } else {
      assert(c!=Cmds::Esc);
      if (c==Cmds::Enter) {
        //TODO: check enabled!
        return I::template cmd<c,It,Nav>(it,nav);
      }
      Empty<>::template cmd<c,It,Nav>(it,nav);
      return true;
    }
  }
};

template<typename I>
struct StaticData<I>:I {
  using This=StaticData<I>;
  inline static constexpr Idx size() {return 1;}
  inline static constexpr Idx size(Ref ref) {
    return ref.len?size(ref,ref.head()):size();
  }
  inline static constexpr Idx size(Ref ref,Idx n) {
    return n?0:ref.len?I::size(ref.tail(),ref.head()):I::size()+1;
  }
  using I::enabled;
  inline bool enabled(Idx n) {return n?true:enabled();}

  using I::enable;
  inline void enable(Idx n,bool b) {if(!n) I::enable(b);}


  template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
  inline void printItem(It& it,Nav& nav,Idx n,Idx p=0) {
    if (!Out::freeY()) return;
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
    it.template fmt<P,true,It,Out,Nav>(0,nav);
    printItem<This,Nav,Out>(*this,nav,Out::top(),Out::top());
    it.template fmt<P,false,It,Out,Nav>(0,nav);
  }

  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    if (n) return;
    else if (ref.len) reinterpret_cast<I*>(this)->I::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav,ref.tail(),ref.head());
    else Out::template printMenu<I,Nav,Out>(*reinterpret_cast<I*>(this),nav);
  }
  template<Cmds c,typename It,typename Nav>
  inline bool cmd(It& it,Nav& nav,Ref ref,Idx n) {
    assert(c!=Cmds::Esc);
    if (c==Cmds::Enter) {
      //TODO: check enabled!
      return I::template cmd<c,It,Nav>(it,nav);
    }
    Empty<>::template cmd<c,It,Nav>(it,nav);
    return true;
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
  inline bool cmd(It& it,Nav& nav) {
    assert(c!=Cmds::Esc);
    nav.open();
    return true;
  }

  using B::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(It& it,Nav& nav,Ref ref,Idx n) {
    if (ref.len) B::template printMenu<It,Nav,Out>(it,nav,ref,n);
    else Out::template printMenu<This,Nav,Out>(*this,nav);
  }
};
