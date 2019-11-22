/* -*- C++ -*- */
#pragma once

#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

//do not use vitual inheritance, memory cost is too high to share a byte
// struct MutBits {
//   Idx isEnabled:1, hasChanged:1;
// };

template<typename I>
struct Mutable:I/*,virtual MutBits*/ {
  bool hasChanged=false;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
};

/**
* The Item class encapsulates a composition to be a menu item.
*/
template<typename I>
struct Item:I/*Mutable<I>*/ {
  using Base=I;//Mutable<I>;
  using This=Item<I>;
  template<typename Nav,typename Out,Roles P=Roles::Item,bool toPrint=true>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool=true) {
    out.template printItem<This,toPrint>(*this,idx,nav.selected(idx),I::enabled(),nav.mode());
  }
  template<typename Out,Roles P=Roles::Item,bool toPrint=true>
  inline void printItem(Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    I::template printItem<This,Out,P,toPrint>(*this,out,n,s,e,m);
  }
  using I::canNav;
  inline bool canNav(Ref ref,Idx n) const {return I::canNav();}
  using I::isMenu;
  inline bool isMenu(Ref ref,Idx n) const {return I::isMenu();}
  using I::activate;
  inline static bool activate(Ref,Idx=0) {return I::activate();}
  using I::cmd;
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {cmd<c,Nav>(nav);}
  using I::parentDraw;
  inline constexpr bool parentDraw(Ref,Idx) {return I::parentDraw();}
};

/**
* The Action class associates an actikon function with a menu item.
*/
template<typename I,ActionHandler act>
struct Action:I {
  using Base=I;
  using I::I;
  using This=Action<I,act>;
  inline static bool activate() {return act();}
  // inline static bool activate(Ref,Idx=0) {return activate();}
};

template<const char**text,typename I=Empty>
struct StaticText:I {
  using Base=I;
  template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void print(Out& out) {
    out.template print<decltype(text[0]),Out,toPrint>(text[0],out,role);
  }
};

template<typename I=Empty>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void print(Out& out) {
    out.template print<decltype(text),Out,toPrint>(text,out,role);
  }
};

template<typename I>
struct Exit:I {
  using Base=I;
  inline static constexpr bool activate() {return false;}
  inline static constexpr bool activate(Ref,Idx=0) {return activate();}
};

//wrap an item with static prefix/suffix content, behaves like the wrapped item
template<typename Of,typename Prefix,typename Suffix=Empty>
struct StaticWrap:Of {
  using Base=Of;
  using Of::Of;
  using Of::print;
  Prefix prefix;
  Suffix suffix;
  template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void print(Out& out) {
    prefix.template print<Out,role,toPrint>(out);
    Of::template print<Out,role,toPrint>(out);
    suffix.template print<Out,role,toPrint>(out);
  }
  template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    prefix.template printItem<Prefix,Out,role,toPrint>(prefix,out,n,s,e,m);
    Of::template printItem<Of,Out,role,toPrint>(*this,out,n,s,e,m);
    suffix.template printItem<Suffix,Out,role,toPrint>(suffix,out,n,s,e,m);
  }
};

// this messes the wrap of parts
template<typename I>
struct ChainPrint:I {
  template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void print(Out& out) {
    I::print(out);
    I::Base::print(out);
  }
  // template<typename It,typename Out,Roles P=Roles::Item,bool toPrint=true>
  // static inline It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   I::printItem(it,out,n,s,e,m);
  //   I::Base::printItem(it,out,n,s,e,m);
  // }
};

//prints all items in sequencde but behaves like the first
// template<typename I,typename... II>
// struct Seq:I {
//   Seq<II...> next;
//   using I::print;
//   template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
//   inline void print(Out& out) {
//     I::print(out);
//     next.print(out);
//   }
//   template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
//   inline It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
//     I::template printItem<It,Out,role>(it,out,n,s,e,m);
//     next.template printItem<It,Out,role>(it,out,n,s,e,m);
//   }
// };
//
// template<typename I>
// struct Seq<I>:I {};

template<typename F,typename S=Empty>
struct Pair:F {
  using Base=F;
  using This=Pair<F,S>;
  using F::F;
  S tail;
  //size ---------------------------------------------------------------------
  inline constexpr Idx size() const {return tail.size()+1;}
  inline constexpr Idx size(Ref ref) const {return ref.len?size(ref,ref.head()):size();}
  inline constexpr Idx size(Ref ref,Idx n) const {return n?tail.size(ref,n-1):ref.len>1?size(ref.tail(),ref.tail().head()):F::size();}
  //enable -------------------------------------------------------------------
  using F::enable;
  inline void enable(bool e,Idx n) {return n?tail.enable(e,n-1):F::enable(e);}
  inline void enable(bool e,Ref ref) {if (ref) enable(e,ref,ref.head());}
  inline void enable(bool e,Ref ref,Idx n) {
    if(n) tail.enable(e,ref,n-1);
    else if(ref.len>1) enable(e,ref.tail(),ref.tail().head());
    else F::enable(e);
  }
  using F::enabled;
  inline bool enabled(Idx n) const {return n?tail.enabled(n-1):F::enabled();}
  inline bool enabled(Ref ref) const {return ref?enabled(ref,ref.head()):enabled();}
  inline bool enabled(Ref ref,Idx n) const {return n?tail.enabled(ref,n-1):ref?enabled(ref.tail(),ref.tail().head()):F::enabled();}


  // commands ------------------------------------------------------------------
  using F::cmd;
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref) {cmd(nav,ref,ref.head());}
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {
    if (n) tail.template cmd<c,Nav>(nav,ref,n-1);
    else if(ref) F::template cmd<c,Nav>(nav,ref.tail(),ref.tail().head());
    else F::template cmd<c,Nav>(nav);
  }

  // print ---------------------------------------------------------------------
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (n) tail.printMenu(pd,*this,nav,out,ref,n-1);
    else if (ref) F::printMenu(pd,*this,nav,out,ref.tail(),ref.tail().head());
    else out.printMenu(*reinterpret_cast<F*>(this),nav);
  }
  template<typename Nav,typename Out,Roles P=Roles::Item,bool toPrint=true>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    if (!out.freeY()) return;
    if(top) tail.printItems(nav,out,idx+1,top-1);
    else {
      out.template printItem<F,toPrint>(*this,idx,nav.selected(idx),F::enabled(),nav.mode());
      tail.printItems(nav,out,idx+1,top,fullPrint);
    }
  }

  using F::activate;
  inline bool activate(Ref ref,Idx n) {
    if (n) return tail.activate(ref,n-1);
    if (ref) return F::activate(ref.tail(),ref.tail().head());
    return F::activate();
  }

  using F::canNav;
  inline bool canNav(Ref ref,Idx n) {
    if (n) return tail.canNav(ref,n-1);
    if (ref) return F::canNav(ref.tail(),ref.tail().head());
    return F::canNav();
  }
  using F::isMenu;
  inline bool isMenu(Ref ref,Idx n) {
    if (n) return tail.isMenu(ref,n-1);
    if (ref) return F::isMenu(ref.tail(),ref.tail().head());
    return F::isMenu();
  }
  using F::parentDraw;
  inline bool parentDraw() const {return F::parentDraw();}
  inline bool parentDraw(Idx n) const {return n?tail.parentDraw(n-1):F::parentDraw();}
  inline bool parentDraw(Ref ref,Idx n) const {return n?tail.parentDraw(n-1):ref?F::parentDraw(ref.tail(),ref.tail().head()):F::parentDraw();}
};

template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
template<typename O>                struct StaticData<O>:Pair<O> {};

template<typename Title,typename Body>
struct StaticMenu:/*Pair<Title,Body>*/Mutable<Pair<Title,Body>>{
  using Base=Mutable<Pair<Title,Body>>;
  // using Base=Pair<Title,Body>;
  using This=StaticMenu<Title,Body>;

  // cmd ---------------------------------------------------
  // using Title::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out) {out.printMenu(pd,*this,nav);}
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref) {
    if (pd&&ref.len==1) out.printParent(*this,nav);
    else if(ref) Base::tail.printMenu(pd,*this,nav,out,ref,ref.head());
    else out.printMenu(*this,nav);
  }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (pd&&ref.len==1) out.printParent(*this,nav);
    else if (ref) Base::printMenu(pd,*this,nav,out,ref,ref.head());
    else out.printMenu(*this,nav);
  }
  template<typename Nav,typename Out>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    Base::tail.template printItems<Nav,Out>(nav,out,idx,top,This::changed());
  }

  // using Title::size;
  inline constexpr Idx size() const {return Base::tail.size();}
  inline constexpr Idx size(Ref ref) const {return Base::tail.size(ref);}
  inline constexpr Idx size(Ref ref,Idx n) const {return Base::tail.size(ref,n);}

  using Title::enable;
  inline void enable(bool e,Idx n) {Base::tail.enable(e,n);}
  inline void enable(bool e,Ref ref) {Base::tail.enable(e,ref,ref.head());}
  using Title::enabled;
  inline bool enabled(Idx n) const {return Base::tail.enabled(n);}
  inline bool enabled(Ref ref) const {return Base::tail.enabled(ref);}

  // cmd ---------------------------------------------------
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav) {nav.template _cmd<c>();}
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref) {
    if(ref) Base::tail.template cmd<c,Nav>(nav,ref,ref.head());
    else nav.template _cmd<c>();
  }
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {nav.template _cmd<c>();}

  inline static constexpr bool canNav() {return true;}
  inline bool canNav(Ref ref,Idx n) {return ref?Base::tail.canNav(ref,n):canNav();}

  inline static constexpr bool isMenu() {return true;}
  inline bool isMenu(Ref ref,Idx n) {return ref?Base::tail.isMenu(ref,n):isMenu();}

  inline static constexpr bool activate() {return true;}
  inline bool activate(Ref ref,Idx n) {return ref?Base::tail.activate(ref,n):activate();}

  inline static constexpr bool parentDraw() {return false;}
  inline bool parentDraw(Ref ref) const {return ref?Base::tail.parentDraw(ref,ref.head()):false;}
  inline bool parentDraw(Ref ref,Idx n) const {return Base::tail.parentDraw(ref,n);}
};
