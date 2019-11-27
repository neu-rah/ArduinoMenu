/* -*- C++ -*- */
#pragma once

#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

//do not use vitual inheritance, memory cost is too high to share a byte
// struct MutBits {
//   Idx isEnabled:1, hasChanged:1;
// };

//mutable is to be used on items but it is needed when using partial draw devices
// guess we need <λ++> here...
// well Mitable will be needed for other cases, considering making it a defualt
template<typename I>
struct Mutable:I/*,virtual MutBits*/ {
  bool hasChanged=true;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  inline static void changed(Idx,bool o) {assert(false);}
};

/**
* The Item class encapsulates a composition to be a menu item.
*/
template<typename I>
struct Item:Mutable<I> {
  using Base=Mutable<I>;
  using This=Item<I>;
  //terminate items printing by printing just this last one as a single item
  template<typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    assert(op!=OutOp::ClearChanges);
    out.template printItem<This,op!=OutOp::Measure>(*this,idx,nav.selected(idx),I::enabled(),nav.mode());
  }

  template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    I::template printItem<This,Out,role,toPrint>(*this,out,n,s,e,m);
  }
  template<typename Out,Roles P=Roles::Item,bool toPrint=true>
  inline void printItem(Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    This::template printItem<This,Out,P,toPrint>(*this,out,n,s,e,m);
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
  template<typename It,typename Out,Roles role=Roles::Raw,bool toPrint=true>
  inline void printItem(It& it,Out& out,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    prefix.template printItem<Prefix,Out,role,toPrint>(prefix,out,n,s,e,m);
    Of::template printItem<Of,Out,role,toPrint>(*this,out,n,s,e,m);
    suffix.template printItem<Suffix,Out,role,toPrint>(suffix,out,n,s,e,m);
  }
};

// template<typename I>
// struct ChainPrint:I {
//   template<typename Out,Roles role=Roles::Raw,bool toPrint=true>
//   inline void print(Out& out) {
//     I::print(out);
//     I::Base::print(out);
//   }
// };

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
  inline void enable(bool e,Idx n) {n?tail.enable(e,n-1):F::enable(e);}
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

  inline void changed(Idx n,bool o) {
    if (n) tail.changed(n-1,o);
    else F::changed(o);
  }

  // commands ------------------------------------------------------------------
  using F::cmd;
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {
    if (n) tail.template cmd<c,Nav>(nav,ref,n-1);
    else if(ref) F::template cmd<c,Nav>(nav,ref.tail(),ref.tail().head());
    else {
      assert(false);
      F::template cmd<c,Nav>(nav);
    }
  }

  // print ---------------------------------------------------------------------
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (n) tail.printMenu(pd,*this,nav,out,ref,n-1);
    else if (ref) F::printMenu(pd,*this,nav,out,ref.tail(),ref.tail().head());
    else out.printMenu(*reinterpret_cast<F*>(this),nav);
  }
  template<typename Nav,typename Out,Roles P/*=Roles::Item*/,OutOp op/*=OutOp::Printing*/>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    if (!out.freeY()) return;
    if(top) tail.template printItems<Nav,Out,P,op>(nav,out,idx+1,top-1,fullPrint);//skip scroll-out part
    else {
      switch(op) {
        case OutOp::Measure:
          out.template printItem<F,false>(*this,idx,nav.selected(idx),F::enabled(),nav.mode());
          break;
        case OutOp::Printing:
          if (fullPrint||F::changed()||!out.partialDraw())
            out.template printItem<F,true>(*this,idx,nav.selected(idx),F::enabled(),nav.mode());
          else
            out.template printItem<F,false>(*this,idx,nav.selected(idx),F::enabled(),nav.mode());
          break;
        case OutOp::ClearChanges:
          F::changed(false);
      }
      tail.template printItems<Nav,Out,P,op>(nav,out,idx+1,top,fullPrint);
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
struct StaticMenu:Mutable<Pair<Title,Body>>{
  using Base=Mutable<Pair<Title,Body>>;
  using This=StaticMenu<Title,Body>;

  // using Title::printMenu;
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out) {out.printMenu(pd,*this,nav);}
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref) {
    This::printMenu(pd,*this,nav,out,ref,ref.head());
  }
  template<typename It,typename Nav,typename Out>
  inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    if (pd&&ref.len==1) out.printParent(*this,nav);
    else if (ref) Base::tail.printMenu(pd,*this,nav,out,ref,ref.head());
    else {
      out.template printMenu<This,Nav,OutOp::Printing>(*this,nav);
      if (out.partialDraw()) out.template printMenu<This,Nav,OutOp::ClearChanges>(*this,nav);
    }
  }
  template<typename Nav,typename Out,Roles role=Roles::Item,OutOp op=OutOp::Printing>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
    Base::tail.template printItems<Nav,Out,role,op>(nav,out,idx,top,fullPrint||(op==OutOp::Printing&&This::changed()));
    // if (op==OutOp::ClearChanges) This::changed(false);
  }

  inline constexpr Idx size() const {return Base::tail.size();}
  inline constexpr Idx size(Ref ref) const {return Base::tail.size(ref);}
  inline constexpr Idx size(Ref ref,Idx n) const {return Base::tail.size(ref,n);}

  using Title::enable;
  inline void enable(bool e,Idx n) {Base::tail.enable(e,n);}
  inline void enable(bool e,Ref ref) {Base::tail.enable(e,ref,ref.head());}
  using Title::enabled;
  inline bool enabled(Idx n) const {return Base::tail.enabled(n);}
  inline bool enabled(Ref ref) const {return Base::tail.enabled(ref);}

  using Base::changed;
  inline void changed(Idx n,bool o) {
    // _trace(MDO<<"StaticMenu::changed("<<n<<")"<<endl);
    Base::tail.changed(n,o);}
  // cmd ---------------------------------------------------
  template<Cmds c,typename Nav>
  void _cmd(Nav& nav) {
    Idx p=nav.pos();
    nav.template _cmd<c>();
    if(p!=nav.pos()) {
      changed(p,true);
      changed(nav.pos(),true);
    }
  }
  template<Cmds c,typename Nav>
  inline static void cmd(Nav& nav) {
    assert(false);
    // _cmd<c,Nav>(nav);
  }
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref) {
    if(ref.len) Base::tail.template cmd<c,Nav>(nav,ref,ref.head());
    else _cmd<c,Nav>(nav);
  }
  template<Cmds c,typename Nav>
  inline void cmd(Nav& nav,Ref ref,Idx n) {
    Idx p=nav.pos();
    nav.template _cmd<c>();
    if(p!=nav.pos()) {
      changed(p,true);
      changed(nav.pos(),true);
    }
  }

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

template<typename I>
class Prompt:public IItem,public I {

};
