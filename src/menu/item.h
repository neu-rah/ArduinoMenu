/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I>
struct Mutable:I {
  bool hasChanged=true;
  using I::I;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  // inline void changed(Idx,bool o) {changed(o);}
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
struct StaticText {
  template<typename I=Empty<Nil>>
  struct Part:I {
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline static void print(Nav& nav,Out& out,PathRef ref=self) {out.raw(text[0]);}
  };
};

template<typename I>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {
    if (op==Op::Printing) out.raw(text);
    I::template print<Nav,Out,op>(nav,out);
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

template<typename F,typename S=Empty<Nil>>
struct Pair:F {
  using Base=F;
  using This=Pair<F,S>;
  using F::F;
  S tail;
  // template<typename Nav,typename Out,Op op=Op::Printing>
  // inline void printMenu(Nav& nav,Out& out,PathRef ref=self);
  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0);
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self);
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref,Idx n);
};

template<typename Title,typename Body>
struct StaticMenu {
  template<typename I>
  struct Part:I {
    using Base=I;
    Title title;
    Body body;
    using This=StaticMenu<Title,Body>;
    using I::I;

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"StaticMenu::printMenu... "<<op<<endl);
      if (ref) body.template printMenu<Nav,Out,op>(nav,out,ref,ref.head());
      else out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
      trace(MDO<<"StaticMenu::printItems"<<endl);
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      trace(MDO<<"StaticMenu::print "<<role<<endl);
      // if (role&(Roles::Title|Roles::Raw))
      title.template print<Nav,Out,op,role>(nav,out,ref);
    }
  };
};

template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
template<typename O>                struct StaticData<O>:Pair<O> {};

template<typename Title,typename Body,Title& title,Body& body>
struct Menu {
  template<typename I>
  struct Part:I {
    using This=Menu<Title,Body,title,body>::Part<I>;
    using I::I;

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self) {
      _trace(MDO<<"Menu::printMenu "<<op<<endl);
      if (ref) body.template printMenu<Nav,Out,op>(nav,out,ref);
      else out.template printMenu<decltype(I::obj()),Nav,op>(I::obj(),nav);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
      _trace(MDO<<"Menu::printItems "<<op<<endl);
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      _trace(MDO<<"Menu::print "<<role<<endl);
      if (role&(Roles::Title|Roles::Raw)) title.print(nav,out);
      // else out.printMenu(I::obj(),nav);
    }
  };
};

// template<typename Title,typename Body,Title& title,Body& body>
// struct Menu {
//   template<typename I=Title>
//   struct Part:Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<Menu<Title,Body,title,body>::As<I>>> {
//     using This=typename Menu<Title,Body,title,body>::template As<I>;
//     using Base=typename Chain<Mutable,Pair<Title,Body>::template As>::template To<Obj<Menu<Title,Body,title,body>::As<I>>>;
//     template<typename Nav,typename Out,Op op=Op::Printing>
//     inline void printMenu(Nav& nav,Out& out) {
//       _trace(MDO<<"Menu::printMenu "<<op<<endl);
//       out.template printMenu<decltype(body),Nav,op>(body,nav);
//     }
//
//     template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
//       body.template printItems<Nav,Out,op,role>(nav,out,idx,top);
//     }
//     template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     inline void print(Nav& nav,Out& out,PathRef ref=self) {
//       _trace(MDO<<"Menu::print "<<role<<endl);
//       if (role&(Roles::Title|Roles::Raw)) title.print(nav,out);
//       // else out.printMenu(I::obj(),nav);
//     }
//   };
//   template<typename O> using Open=As<O>;
// };

struct IItem {
  // virtual inline Idx len(PathRef=self)=0;
  virtual inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing)=0;
  virtual inline void printItems(INav&,IOut&,Idx=0,Idx=0,PathRef=self,Op op=Op::Printing,Roles role=Roles::Raw)=0;
  virtual inline void print(INav&,IOut&,Op op,Roles role,PathRef=self)=0;
  virtual inline bool enabled(PathRef=self)=0;
  virtual inline void enable(bool,PathRef=self)=0;
  virtual inline bool changed() const=0;
  virtual inline void changed(bool o)=0;

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self)
    {printItems(nav,out,idx,top,ref,op,role);}
  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out) {printMenu(nav,out,op);}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {print(nav,out,op,role,ref);}
};

template<Expr... I>
struct Prompt:IItem,Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>> {
  using Base=typename Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>>;
  using This=Prompt<I...>;
  using Base::Base;
  using Base::printItems;
  // using Base::printMenu;
  using Base::print;
  // inline Idx len(PathRef ref=self) override {return O::len(ref);}
  inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing) override;
  inline void printItems(INav& nav,IOut& out,Idx idx=0,Idx top=0,PathRef ref=self,Op op=Op::Printing,Roles role=Roles::Raw) override;
  inline void print(INav& nav,IOut& out,Op op,Roles role,PathRef ref=self) override;
  inline bool enabled(PathRef ref=self) override {return Base::enabled(ref);}
  inline void enable(bool o,PathRef ref=self) override {Base::enable(o,ref);}
  virtual inline bool changed() const {return Base::changed();}
  virtual inline void changed(bool o) {Base::changed(o);}
};

template<typename I>
struct Exit:I {
  using Base=I;
  inline static constexpr bool activate() {return false;}
  inline static constexpr bool activate(PathRef,Idx=0) {return activate();}
};
