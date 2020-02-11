/* -*- C++ -*- */
#pragma once

#include "api.h"

template<class I>
struct Mutable:I {
  using I::I;
  inline bool changed() const {return hasChanged;}
  inline void changed(bool o) {hasChanged=o;}
  // inline void changed(Idx,bool o) {changed(o);}

  //this can not be `protected` because of `CRTP` and `mixin` composition
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {
    if (!ref&&op==Op::ClearChanges) {
      _trace(MDO<<"Mutable::clear "<<ref<<endl);
      changed(false);
    } else I::template print<Nav,Out,op,role>(nav,out,ref);
  }
protected:
  bool hasChanged=true;
};

/// ActionHanlder, type of action functions to associate with items
using ActionHandler=bool (*)();

/**
* The Action class associates an actikon function with a menu item.
*/
template<ActionHandler act>
struct Action {
  template<typename I>
  struct Part:I {
    using Base=I;
    using I::I;
    using This=Action<act>::Part<I>;
    inline static bool activate(PathRef ref=self) {return ref?false:act();}
  };
};

template<const char** text>
struct StaticText {
  template<typename I=Empty<Nil>>
  struct Part:I {
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      out.template raw<decltype(text[0]),op==Op::Printing>(text[0]);
      I::template print<Nav,Out,op,role>(nav,out);
    }
  };
};

template<typename I>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {
    out.template raw<decltype(text),op==Op::Printing>(text);
    I::template print<Nav,Out,op,role>(nav,out);
  }
  template<typename Nav,typename Out,Roles role=Roles::Raw>
  inline void measure(Nav& nav,Out& out,PathRef ref=self) {
    out.measure(text);
    I::template measure<Nav,Out,role>(nav,out);
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
  using F::changed;

  inline void changed(Idx n,bool o) {
    if (n) tail.changed(n-1,o);
    else F::changed(o);
  }

  inline bool enabled(PathRef ref=self,Idx n=0) const {
    trace(MDO<<"Pair::enabled "<<ref<<" "<<n<<endl);
    if(n) return tail.enabled(ref,--n);
    if (ref) return F::enabled(ref.tail());
    return F::enabled();
  }
  inline void enable(bool b,PathRef ref=self,Idx n=0) {
    trace(MDO<<"Pair::enable "<<ref<<" "<<n<<endl);
    if(n) tail.enable(b,ref,--n);
    else if (ref) F::enable(b,ref.tail());
    else F::enable(n);
  }
  inline Idx size(PathRef ref=self,Idx n=0) const {
    trace(MDO<<"Pair::size "<<ref<<" "<<n<<endl);
    if(n) return tail.size(ref,--n);
    if (ref) return F::size(ref.tail());
    return tail.size()+1;
  }
  inline bool canNav(PathRef ref=self,Idx n=0) {
    trace(MDO<<"Pair::canNav "<<ref<<" "<<n<<endl);
    if(n) return tail.canNav(ref,--n);
    if (ref) return F::canNav(ref.tail());
    return F::canNav();
  }

  inline bool activate(PathRef ref=self,Idx n=0) {
    trace(MDO<<"Pair::activate "<<ref<<" "<<n<<endl);
    if(n) return tail.activate(ref,--n);
    if (ref) return F::activate(ref.tail());
    return F::activate();
  }

  template<Cmd c,typename Nav>
  inline bool cmd(Nav& nav,PathRef ref=self,Idx n=0) {
    if(n) return tail.template cmd<c,Nav>(nav,ref,--n);
    if (ref) return F::template cmd<c,Nav>(nav,ref.tail());
    return F::template cmd<c,Nav>(nav);
  }

  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0);
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self,bool fullPrint=true);
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

    inline bool enabled(PathRef ref=self) const {
      return ref?body.enabled(ref,ref.head()):Base::enabled();
    }

    inline void enable(bool b,PathRef ref=self) {
      if(ref) body.enable(b,ref,ref.head());
      else Base::enable(b);
    }

    inline size_t size(PathRef ref=self) const {
      trace(MDO<<"StaticMenu::Part::size -> body.size() "<<ref<<endl);
      return ref?body.size(ref,ref.head()):body.size();
    }

    inline bool canNav(PathRef ref=self) {
      trace(MDO<<"StaticMenu::canNav "<<ref<<endl);
      return ref?body.canNav(ref,ref.head()):true;
    }

    inline bool activate(PathRef ref=self) {
      trace(MDO<<"StaticMenu::activate "<<ref<<endl);
      return ref?body.activate(ref,ref.head()):true;
    }

    inline bool parentPrint(PathRef ref=self) {
      trace(MDO<<"StaticMenu::parentPrint "<<ref<<endl);
      return ref?body.parentPrint(ref,ref.head()):false;
    }

    inline void changed(Idx n,bool o) {body.changed(n,o);}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,PathRef ref=self) {
      Idx p=nav.pos();
      bool res=ref?body.template cmd<c,Nav>(nav,ref,ref.head()):body.template cmd<c,Nav>(nav);
      if(p!=nav.pos()) {
        changed(p,true);
        changed(nav.pos(),true);
      }
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"StaticMenu::printMenu... "<<op<<" "<<ref<<endl);
      out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
      // if (out.partialDraw()) {
      //   trace(MDO<<"partial draw cleanup!"<<endl);
      //   out.template printMenu<typename I::Type,Nav,Op::ClearChanges>(I::obj(),nav);
      // }
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self,bool fullPrint=true) {
      trace(MDO<<"StaticMenu::printItems fullPrint:"<<fullPrint<<endl);
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref,fullPrint);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      trace(MDO<<"StaticMenu::print "<<role<<endl);
      if (Base::obj().changed()||title.changed()) title.template print<Nav,Out,op,role>(nav,out,ref);
    }
  };
};

template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
template<typename O>                struct StaticData<O>:Pair<O> {};
