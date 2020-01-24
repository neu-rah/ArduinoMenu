/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I>
struct Mutable:I {
  bool hasChanged=true;
  using I::I;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  inline void changed(Idx,bool o) {changed(o);}
};

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<ActionHandler act,typename I>
struct Action:I {
  using Base=I;
  using I::I;
  using This=Action<act,I>;
  inline static bool activate() {return act();}
  // inline static bool activate(PathRef ref,Idx n) {return ref?false:act();}
};

template<const char** text>
struct StaticText:Obj<StaticText<text>> {
  template<typename I>
  struct As:I {
    template<typename Out,Op op=Op::Printing>
    inline static void print(Out& out) {out.raw(text[0]);}
  };
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
struct Pair {
  template<typename I>
  struct As:I {
    using Base=I;
    using This=Pair<F,S>::As<I>;
    using I::I;
    S tail;
    //size ---------------------------------------------------------------------
    inline constexpr Idx size() const {return tail.size()+1;}
  };
};

template<typename Title,typename Body,Title& title,Body& body>
struct StaticMenu {
  template<typename I=Title>
  struct As:Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<StaticMenu<Title,Body,title,body>::As<I>>> {
    using This=typename StaticMenu<Title,Body,title,body>::template As<I>;
    using Base=typename Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<StaticMenu<Title,Body,title,body>::As<I>>>;
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out) {
      out.template printMenu<typename This::Type,Nav,op>(This::obj(),nav);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top);
    }
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out) {
      if (role&(Roles::Title|Roles::Raw)) I::print(out);
      // else out.printMenu(I::obj(),nav);
    }
  };
  template<typename O> using Open=As<O>;
};
