/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {

  struct Mutable {
    template<class I>
    struct Part:I {
      using I::I;
      inline bool changed() const {return hasChanged;}
      inline void changed(bool o) {
        trace(MDO<<"Mutable "<<(o?"dirt":"clear")<<endl);
        hasChanged=o;}
      // inline void changed(Idx,bool o) {changed(o);}

      //this can not be `protected` because of `CRTP` and `mixin` composition
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        if (op==Op::ClearChanges&&!ref) {
          _trace(MDO<<"Mutable::print clear "<<endl);
          changed(false);
        }// else
        I::template print<Nav,Out,op>(nav,out,ref);
      }
    protected:
      bool hasChanged=true;
    };
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
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        out.template raw<decltype(text[0]),op==Op::Printing>(text[0]);
        I::template print<Nav,Out,op>(nav,out);
      }
    };
  };

  struct Text {
    template<typename I>
    struct Part:I {
      using Base=I;
      const char* text;
      inline Part(const char*o):text(o) {}
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        bool toPrint=op==Op::Printing&&out.fullDraw()&&I::obj().changed();
        if(toPrint) out.template raw<decltype(text),true>(text);
        else out.template raw<decltype(text),false>(text);
        I::template print<Nav,Out,op>(nav,out);
      }
      // template<typename Nav,typename Out,Roles role=Roles::Raw>
      // inline void measure(Nav& nav,Out& out,PathRef ref=self) {
      //   out.measure(text);
      //   I::template measure<Nav,Out,role>(nav,out);
      // }
    };
  };

  /**
  * The Item class encapsulates a composition to be a stratic menu item.
  */
  template<Expr... I>
  struct Item:Chain<I...,Empty>::template To<Obj<Item<I...>>> {
    using Base=typename Chain<I...,Empty>::template To<Obj<Item<I...>>>;
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
      if (ref.len==1&&!F::enabled()) return !F::canNav();
      if (ref) return F::activate(ref.tail());
      trace(MDO<<"F::enabled "<<F::enabled()<<endl);
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

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self);
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
        return ref?body.activate(ref,ref.head()):enabled();
      }

      inline bool parentPrint(PathRef ref=self) {
        trace(MDO<<"StaticMenu::parentPrint "<<ref<<endl);
        return ref?body.parentPrint(ref,ref.head()):false;
      }

      using Base::changed;
      inline void changed(Idx n,bool o) {body.changed(n,o);}

      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,PathRef ref=self) {
        Idx p=nav.pos();
        bool res=ref?body.template cmd<c,Nav>(nav,ref,ref.head()):body.template cmd<c,Nav>(nav);
        if(p!=nav.pos()) {
          changed(p,true);
          changed(nav.pos(),true);
        }
        return res;
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
        trace(MDO<<"StaticMenu::printMenu... "<<op<<" "<<ref<<endl);
        if(ref.len>1&&body.parentPrint(ref)) body.template printMenu<Nav,Out,op>(nav,out,ref.tail(),ref.head());
        else {
          bool fullPrint=out.fullDraw()/*||Base::obj().changed()*/||!out.isSame(&Base::obj());
          if (fullPrint) out.template printMenu<typename I::Type,Nav,true,op>(Base::obj(),nav);
          else out.template printMenu<typename I::Type,Nav,false,op>(Base::obj(),nav);
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self) {
        trace(MDO<<"StaticMenu::printItems fullPrint:"<<fullPrint<<endl);
        body.template printItems<Nav,Out,op>(nav,out,fullPrint,idx,top,ref);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out out,bool fullPrint) {
        trace(MDO<<"StaticMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
          out.template printTitle<typename I::Type,Nav,op>(I::obj(),nav);
        else
          out.template printTitle<typename I::Type,Nav,op==Op::Printing?Op::Measure:op>(I::obj(),nav);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void print(Nav& nav,Out& out,PathRef ref=self) {
        trace(MDO<<"StaticMenu::print "<<op<<endl);
        title.template print<Nav,Out,op>(nav,out,ref);
      }
    };
  };

  template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
  template<typename O>                struct StaticData<O>:Pair<O> {};
};
