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
  // inline bool isMenu(PathRef ref=self,Idx n=0) {
  //   trace(MDO<<"Pair::isMenu "<<ref<<" "<<n<<endl);
  //   if(n) return tail.isMenu(ref,--n);
  //   if (ref) return F::isMenu(ref.tail());
  //   return F::isMenu();
  // }
  inline bool activate(PathRef ref=self,Idx n=0) {
    _trace(MDO<<"Pair::activate "<<ref<<" "<<n<<endl);
    if(n) return tail.activate(ref,--n);
    if (ref) return F::activate(ref.tail());
    return F::activate();
  }
  inline bool parentPrint(PathRef ref=self,Idx n=0) {
    trace(MDO<<"Pair::parentPrint "<<ref<<" "<<n<<endl);
    if(n) return tail.parentPrint(ref,--n);
    if (ref) return F::parentPrint(ref.tail());
    return false;
  }
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

    inline size_t size(PathRef ref=self) const {
      trace(MDO<<"StaticMenu::Part::size -> body.size() "<<ref<<endl);
      return ref?body.size(ref,ref.head()):body.size();
    }

    inline bool canNav(PathRef ref=self) {
      trace(MDO<<"StaticMenu::canNav "<<ref<<endl);
      return ref?body.canNav(ref,ref.head()):true;
    }

    // inline bool isMenu(PathRef ref=self) {
    //   trace(MDO<<"StaticMenu::isMenu "<<ref<<endl);
    //   return ref?body.isMenu(ref,ref.head()):true;
    // }

    inline bool activate(PathRef ref=self) {
      _trace(MDO<<"StaticMenu::activate "<<ref<<endl);
      return ref?body.activate(ref,ref.head()):true;
    }

    inline bool parentPrint(PathRef ref=self) {
      trace(MDO<<"StaticMenu::parentPrint "<<ref<<endl);
      return ref?body.parentPrint(ref,ref.head()):false;
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"StaticMenu::printMenu... "<<op<<" "<<ref<<endl);
      if (!ref||ref.len==1&&body.parentPrint(ref))
        out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
      else body.template printMenu<Nav,Out,op>(nav,out,ref,ref.head());
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
      trace(MDO<<"StaticMenu::printItems"<<endl);
      body.template printItems<Nav,Out,op,role>(nav,out,idx,top,ref);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {
      trace(MDO<<"StaticMenu::print "<<role<<endl);
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

    inline size_t size(PathRef ref=self) const {return body.size(ref);}

    inline bool canNav(PathRef ref=self) {
      trace(MDO<<"StaticMenu::canNav "<<ref<<endl);
      return ref?body.canNav(ref,ref.head()):true;
    }

    // inline bool isMenu(PathRef ref=self) {
    //   trace(MDO<<"StaticMenu::isMenu "<<ref<<endl);
    //   return ref?body.isMenu(ref,ref.head()):true;
    // }

    inline bool activate(PathRef ref=self) {
      _trace(MDO<<"StaticMenu::activate "<<ref<<endl);
      return ref?body.activate(ref,ref.head()):true;
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self/*,Idx n=0*/) {
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
      title.print(nav,out);
    }
  };
};

struct IItem {
  virtual inline size_t size(PathRef=self)=0;
  virtual inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing,PathRef=self)=0;
  virtual inline void printItems(INav&,IOut&,Idx=0,Idx=0,PathRef=self,Op op=Op::Printing,Roles role=Roles::Raw)=0;
  virtual inline void print(INav&,IOut&,Op op,Roles role,PathRef=self)=0;
  virtual inline bool enabled(PathRef=self)=0;
  virtual inline void enable(bool,PathRef=self)=0;
  virtual inline bool changed() const=0;
  virtual inline void changed(bool o)=0;
  virtual inline bool parentPrint(PathRef ref=self)=0;

  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self)
    {printItems(nav,out,idx,top,ref,op,role);}
  template<typename Nav,typename Out,Op op=Op::Printing>
  inline void printMenu(Nav& nav,Out& out,PathRef ref=self) {printMenu(nav,out,op,ref);}
  template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  inline void print(Nav& nav,Out& out,PathRef ref=self) {print(nav,out,op,role,ref);}
};

template<Expr... I>
struct Prompt:IItem,Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>> {
  using Base=typename Chain<Mutable,I...,Empty>::template To<Obj<Prompt<I...>>>;
  using This=Prompt<I...>;
  using Base::Base;
  using Base::printItems;
  using Base::printMenu;
  using Base::print;
  inline size_t size(PathRef ref=self) override {return Base::size(ref);}
  inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing,PathRef ref=self) override;
  inline void printItems(INav& nav,IOut& out,Idx idx=0,Idx top=0,PathRef ref=self,Op op=Op::Printing,Roles role=Roles::Raw) override;
  inline void print(INav& nav,IOut& out,Op op,Roles role,PathRef ref=self) override;
  inline bool enabled(PathRef ref=self) override {return Base::enabled(ref);}
  inline void enable(bool o,PathRef ref=self) override {Base::enable(o,ref);}
  inline bool changed() const override {return Base::changed();}
  inline void changed(bool o) override {Base::changed(o);}
  inline bool parentPrint(PathRef ref=self) override {return Base::parentPrint(ref);}
};

template<typename I>
struct Exit:I {
  using Base=I;
  inline static constexpr bool activate() {return false;}
  inline static constexpr bool activate(PathRef,Idx=0) {return activate();}
};
