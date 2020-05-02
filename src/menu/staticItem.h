/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {

  template<Idx idTag>
  struct IdTag {
    template<typename I>
    struct Part:I {
      using I::I;
      template<Idx tag> static inline constexpr bool id() {return idTag==tag;}
      static inline bool id(Idx tag) {return idTag==tag;}
    };
  };

  struct Mutable {
    template<class I>
    struct Part:I {
      using I::I;
      using I::changed;
      inline bool changed() const {return hasChanged;}
      inline void changed(bool o) {hasChanged=o;}

      //this can not be `protected` because of `CRTP` and `mixin` composition
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out) {
        if (op==Op::ClearChanges) {
          trace(MDO<<"Mutable::print clear "<<endl);
          changed(false);
        }
        if (delegate) I::template print<Nav,Out,op>(nav,out);
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
      inline static ActRes activate() {
        return act()?
          (I::canNav()?ActRes::Open:ActRes::Stay):
          (I::canNav()?ActRes::Stay:ActRes::Close);
      }
    };
  };

  template<const char** text>
  struct StaticText {
    template<typename I=Empty<Nil>>
    struct Part:I {
      using I::I;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out) {
        out.template raw<decltype(text[0]),op==Op::Printing>(text[0]);
        if(delegate) I::template print<Nav,Out,op>(nav,out);
      }
    };
  };

  struct Text {
    template<typename I>
    struct Part:I {
      using Base=I;
      const char* text;
      template<typename... OO>
      inline Part(const char*o,OO... oo):text(o),I(oo...) {}
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out) {
        out.template raw<decltype(text),op==Op::Printing>(text);
        if(delegate) I::template print<Nav,Out,op>(nav,out);
      }
    };
  };

  //static numeric field, ranges are statically defined
  template<typename T,T& value,T low,T high,T step,T tune,bool loop=false>
  struct StaticNumField {
    template<typename I>
    struct Part:I {
      using I::I;
      inline static bool stepVal(T delta) {
        if (delta==0) return false;
        if (delta > 0) {
          if (high-value>=delta) value+=delta;
          else value=loop?low:high;
        } else {
          if (value-low>=-delta) value+=delta;
          else value=loop?high:low;
        }
        return true;
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out) {
        out.raw(value);
        if(delegate)I::template print<Nav,Out,op>(nav,out);
      }

      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx=0) {
        trace(MDO<<"Field::cmd "<<c<<" mode:"<<nav.mode()<<endl);
        if(nav.mode()==Mode::Normal&&c!=Cmd::Enter) return I::template cmd<c,Nav>(nav);
        switch(c) {
          case Cmd::Enter:
            switch(nav.mode()) {
              case Mode::Normal: nav.setMode(Mode::Edit);break;
              case Mode::Edit:nav.setMode(tune==0?Mode::Normal:Mode::Tune);break;
              case Mode::Tune: nav.setMode(Mode::Normal);break;
              default: return false;
            }
            return true;
          case Cmd::Esc:
            nav.setMode(Mode::Normal);
            return false;
          case Cmd::Up: stepVal(-(nav.mode()==Mode::Edit?step:tune));break;
          case Cmd::Down: stepVal(nav.mode()==Mode::Edit?step:tune);break;
          default:return false;
        }
        return true;
      }

    protected:
      T shadow;
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

  template<typename F,typename S=Item<Empty>>
  struct Pair:F {
    using Base=F;
    using This=Pair<F,S>;
    using F::F;
    S tail;
    using F::changed;

    template<typename A>
    inline APIRes walkId(A& api,Idx i) {
      return F::id(i)?
        api.template call<F>(F::obj()):
        tail.template walkId<A>(api,i);
    }

    template<typename A,Idx i>
    APIRes walkId(A& api) {
      return F::template id<i>()?
        api.template call<F>(F::obj()):
        tail.template walkId<A,i>(api);
    }

    template<typename A>
    APIRes walkPath(A& api,PathRef ref) {
      return walkPath(api,ref,ref.head());
    }
    template<typename A>
    APIRes walkPath(A& api,PathRef ref,Idx n) {
      trace(MDO<<"Pair::walkPath "<<api.named()<<ref<<n<<endl);
      if (n) return tail.template walkPath<A>(api,ref,n-1);
      if (api.chk(F::obj(),ref)) return F::template walkPath<A>(api,ref.tail());
      return api.template call<F>(F::obj()/*,ref.head()*/);
    }

    inline constexpr size_t size() const {return tail.size()+1;}

    using F::parentPrint;
    inline bool parentPrint(Idx n=0) {return n?tail.parentPrint(n-1):F::parentPrint();}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx n=0) {
      trace(MDO<<"Pair::cmd "<<c<<" n:"<<n<<endl);
      if(n) return tail.template cmd<c,Nav>(nav,n-1);
      return F::template cmd<c,Nav>(nav);
    }

    inline void changed(Idx i,bool o) {
      if (i) tail.changed(i-1,o);
      else F::changed(o);
    }

    inline bool enabled(PathRef ref) const {
      return walkPath(APICall::Enabled(),ref);
    }

    inline void enable(bool b,PathRef ref) {
      if(b) walkPath(APICall::Enable<true>(),ref);
      else walkPath(APICall::Enable<false>(),ref);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
    inline void printMenu(Nav& nav,Out& out) {
      out.template printMenu<typename F::Type,Nav,op>
        (F::obj(),nav,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0) {
      trace(MDO<<"Pair<...>::printItems out:@"<<(unsigned long)&out<<" out.fullDraw:"<<out.fullDraw()<<endl);
      if (op==Op::Printing&&(fullPrint||out.fullDraw()||F::changed()))
        out.template printItem<typename F::Type,Nav,op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
      else out.template printItem<
          typename F::Type,
          Nav,op==Op::Printing?Op::Measure:op
        >(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
      tail.template printItems<Nav,Out,op>(nav,out,fullPrint,idx+1,top);
    }
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

      template<typename A,Idx n>
      inline APIRes walkId(A& api) {
        return Base::id(n)?
          api.call(Base::obj()):
          body.template walkId<A,n>(api);
      }
      template<typename A>
      inline APIRes walkId(A& api,Idx n) {
        return Base::id(n)?
          api.call(Base::obj()):
          body.walkId(api,n);
      }
      template<typename A>
      APIRes walkPath(A& api,PathRef ref) {
        return api.chk(Base::obj(),ref)?
          body.template walkPath<A>(api,ref,ref.head()):
          api.call(Base::obj());
      }

      using Base::size;
      inline size_t size(PathRef ref) {
        auto f=APICall::Size();
        return walkPath(f,ref);
      }
      using Base::enabled;
      inline bool enabled(PathRef ref) {
        auto f=APICall::Enabled();
        return walkPath(f,ref);
      }
      using Base::enable;
      inline void enable(bool b,PathRef ref) {
        // if(ref) body.enable(b,ref/*,ref.head()*/); else Base::enable(b);
        auto en=APICall::Enable<true>();
        auto dis=APICall::Enable<false>();
        if(b) walkPath(en,ref);
        else walkPath(dis,ref);
      }
      inline constexpr size_t size() const {return body.size();}
      inline static constexpr bool canNav() {return true;}
      inline ActRes activate() {
        return ActRes::Open;}
      using Base::parentPrint;
      inline bool parentPrint(Idx n) {return body.parentPrint(n);}

      using Base::changed;
      inline void changed(Idx i,bool o) {body.changed(i,o);}

      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx n) {
        Idx p=nav.pos();
        bool res=body.template cmd<c,Nav>(nav,n);
        if(p!=nav.pos()) {
          changed(p,true);
          changed(nav.pos(),true);
        }
        return res;
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out) {
        trace(MDO<<"StaticMenu::printMenu"<<endl);
        out.template printMenu<typename I::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()));
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0) {
        trace(MDO<<"StaticMenu::printItems"<<endl);
        body.template printItems<Nav,Out,op>(nav,out,fullPrint,idx,top);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out& out,bool fullPrint) {
        trace(MDO<<"StaticMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<" out:"<<((long)&out)<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else {
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
            out.template printTitle<typename I::Type,Nav,op>(I::obj(),nav);
          else
            out.template printTitle<typename I::Type,Nav,op==Op::Printing?Op::Measure:op>(I::obj(),nav);
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out) {
        trace(MDO<<"StaticMenu::print "<<op<<endl);
        title.template print<Nav,Out,op,delegate>(nav,out);
      }

    };
  };

  template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
  template<typename O>                struct StaticData<O>:Pair<O> {};
};
