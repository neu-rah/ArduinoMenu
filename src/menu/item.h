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
  template<typename Nav,typename Out,Roles role=Roles::Raw>
  inline void clear(Nav& nav,Out& out,PathRef ref=self) {
    trace(MDO<<"Mutable::clear "<<ref<<endl);
    changed(false);
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
    template<typename Nav,typename Out,Roles role=Roles::Raw>
    inline static void draw(Nav& nav,Out& out,PathRef ref=self) {out.raw(text[0]);}
    // template<typename Nav,typename Out,Roles role=Roles::Raw>
    // inline static void measure(Nav& nav,Out& out,PathRef ref=self) {
    //   out.use(out.measure(text[0]));
    // }
  };
  template<typename Nav,typename Out,Roles role=Roles::Raw>
  inline static void measure(Nav& nav,Out& out,PathRef ref=self) {out.measure(text[0]);}
};

template<typename I>
struct Text:I {
  using Base=I;
  const char* text;
  inline Text(const char*o):text(o) {}
  template<typename Nav,typename Out,Roles role=Roles::Raw>
  inline void draw(Nav& nav,Out& out,PathRef ref=self) {
    out.raw(text);
    I::template draw<Nav,Out,role>(nav,out);
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
  // inline bool parentPrint(PathRef ref=self,Idx n=0) {
  //   trace(MDO<<"Pair::parentPrint "<<ref<<" "<<n<<endl);
  //   if(n) return tail.parentPrint(ref,--n);
  //   if (ref) return F::parentPrint(ref.tail());
  //   return false;
  // }
  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0);
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self);
  // template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  // inline void printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref,Idx n);

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

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"StaticMenu::printMenu... "<<op<<" "<<ref<<endl);
      // if (!ref||ref.len==1&&body.parentPrint(ref))
        out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
        if (out.partialDraw()) {
          trace(MDO<<"partial draw cleanup!"<<endl);
          out.template printMenu<typename I::Type,Nav,Op::ClearChanges>(I::obj(),nav);
        }
      // else {
      //   body.template printMenu<Nav,Out,op>(nav,out,ref,ref.head());
      //   if (out.partialDraw()) {
      //     _trace(MDO<<"partial draw cleanup!"<<endl);
      //     body.template printMenu<Nav,Out,Op::ClearChanges>(nav,out,ref,ref.head());
      //   }
      // }
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
      trace(MDO<<"StaticMenu::printItems"<<endl);
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref);
    }

    // template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    // inline void print(Nav& nav,Out& out,PathRef ref=self) {
    //   _trace(MDO<<"StaticMenu::print "<<role<<endl);
    //   switch(op) {
    //     case Op::Printing:
    //       title.template print<Nav,Out,op,role>(nav,out,ref);
    //       break;
    //     case Op::ClearChanges: Base::obj().changed(false);break;
    //     case Op::Measure: out.useY();break;
    //   }
    // }
  };
};

template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
template<typename O>                struct StaticData<O>:Pair<O> {};
