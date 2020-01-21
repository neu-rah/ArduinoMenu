/* -*- C++ -*- */
#pragma once

#include "api.h"

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

template<typename I>
struct Mutable:I {
  bool hasChanged=true;
  using I::I;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  inline void changed(Idx,bool o) {changed(o);}
};

/**
* The Action class associates an actikon function with a menu item.
*/
template<ActionHandler act,typename I>
struct Action:I {
  using Base=I;
  using I::I;
  using This=Action<act,I>;
  inline static bool activate() {return act();}
  inline static bool activate(Ref ref,Idx n) {return ref?false:act();}
};

template<const char** text>
struct StaticText:Obj<StaticText<text>> {
  template<typename Out,Op op=Op::Printing>
  inline static void print(Out& out) {out.raw(text[0]);}
};

template<typename I>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Out,Op op=Op::Printing>
  inline void print(Out& out) {
    if (op==Op::Printing) out.raw(text);
    I::template print<Out,op>(out);
  }
};

/**
* The Item class encapsulates a composition to be a stratic menu item.
*/
template<Expr... I>
struct Item:Chain<Mutable,I...,Empty>::template To<Obj<Item<I...>>> {
  using Base=typename Chain<Mutable,I...,Empty>::template To<Obj<Item<I...>>>;
  using This=Item<I...>;
  using Base::Base;
};

template<typename F,typename S>
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
  // using F::enable;
  // inline void enable(bool e,Idx n) {n?tail.enable(e,n-1):F::enable(e);}
  // inline void enable(bool e,Ref ref) {if (ref) enable(e,ref,ref.head());}
  // inline void enable(bool e,Ref ref,Idx n) {
  //   if(n) tail.enable(e,ref,n-1);
  //   else if(ref.len>1) enable(e,ref.tail(),ref.tail().head());
  //   else F::enable(e);
  // }
  // using F::enabled;
  // inline bool enabled(Idx n) const {return n?tail.enabled(n-1):F::enabled();}
  // inline bool enabled(Ref ref) const {return ref?enabled(ref,ref.head()):enabled();}
  // inline bool enabled(Ref ref,Idx n) const {return n?tail.enabled(ref,n-1):ref?enabled(ref.tail(),ref.tail().head()):F::enabled();}
  //
  // using F::changed;
  // inline void changed(Idx n,bool o) {
  //   if (n) tail.changed(n-1,o);
  //   else F::changed(o);
  // }
  //
  // // commands ------------------------------------------------------------------
  // using F::cmd;
  // template<Cmds c,typename Nav>
  // inline void cmd(Nav& nav,Ref ref,Idx n) {
  //   if (n) tail.template cmd<c,Nav>(nav,ref,n-1);
  //   else if(ref) F::template cmd<c,Nav>(nav,ref.tail(),ref.tail().head());
  //   else {
  //     assert(false);
  //     F::template cmd<c,Nav>(nav);
  //   }
  // }
  //
  // // print ---------------------------------------------------------------------
  // template<typename It,typename Nav,typename Out>
  // inline void printMenu(bool pd,It& it,Nav& nav,Out& out,Ref ref,Idx n) {
  //   if (n) tail.printMenu(pd,*this,nav,out,ref,n-1);
  //   else if (ref) F::printMenu(pd,it,nav,out,ref.tail(),ref.tail().head());
  //   else out.printMenu(*reinterpret_cast<F*>(this),nav,out);
  // }
  // template<typename It,typename Nav,typename Out,Roles P/*=Roles::Item*/,OutOp op/*=OutOp::Printing*/>
  // inline void printItems(It& it,Nav& nav,Out& out,Idx idx=0,Idx top=0,bool fullPrint=true) {
  //   trace(
  //     MDO<<"Pair::printItems of:";
  //     it.print(debugOut);
  //     MDO<<endl);
  //   if (!out.freeY()) return;
  //   if(top) tail.template printItems<S,Nav,Out,P,op>(tail,nav,out,idx+1,top-1,fullPrint);//skip scroll-out part
  //   else {
  //     switch(op) {
  //       case OutOp::Measure:
  //         out.template printItem<F,Out,false>(*this,out,idx,nav.selected(idx),F::enabled(),nav.mode());
  //         break;
  //       case OutOp::Printing:
  //         if (fullPrint||F::changed()||!out.partialDraw())
  //           out.template printItem<F,Out,true>(*this,out,idx,nav.selected(idx),F::enabled(),nav.mode());
  //         else
  //           out.template printItem<F,Out,false>(*this,out,idx,nav.selected(idx),F::enabled(),nav.mode());
  //         break;
  //       case OutOp::ClearChanges:
  //         F::changed(false);
  //     }
  //     tail.template printItems<S,Nav,Out,P,op>(tail,nav,out,idx+1,top,fullPrint);
  //   }
};
