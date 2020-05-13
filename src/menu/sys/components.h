/* -*- C++ -*- */
#pragma once

/********************************
Menu system common components
*/

#include "kernel/api.h"


namespace Menu {
  //provide an ID for menu items
  template<Idx idTag>
  struct IdTag {
    template<typename I>
    struct Part:I {
      using I::I;
      template<Idx tag>
      static inline constexpr bool id() {return idTag==tag;}
      static inline bool id(Idx tag) {return idTag==tag;}
    };
  };

  //track item changed state (need draw)
  struct Mutable {
    template<class I>
    struct Part:I {
      using I::I;
      using I::changed;
      inline bool changed() const {return hasChanged;}
      inline void changed(bool o) {hasChanged=o;}

      //this can not be `protected` because of `CRTP` and `mixin` composition
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        if (op==Op::ClearChanges) {
          trace(MDO<<"Mutable::print clear "<<endl);
          changed(false);
        }
        if (delegate) I::template print<Nav,Out,op>(nav,out,level);
      }
    protected:
      bool hasChanged=true;
    };
  };

  /**
  * The Handler class associates an action function with a menu item.
  */
  template<ActionHandler act>
  struct Handler {
    template<typename I>
    struct Part:I {
      using This=Part<I>;
      using Base=I;
      using Base::Base;
      template<typename Nav>
      inline bool action(Nav& nav,Idx level) {
        return act()&&Base::action(nav,level);
      }
    };
  };

  /**
  * The Action class associates an action function with a menu item and fires it on `activate`
  */
  template<ActionHandler act>
  struct Action {
    template<typename I>
    struct Part:Handler<act>::template Part<I> {
      using This=Part<I>;
      using Base=typename Handler<act>::template Part<I>;
      using Base::Base;
      using Base::activate;
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        trace(MDO<<"Action::activate"<<endl);
        if (!Base::obj().enabled()) return;
        if(This::obj().action(nav,level)) {if(Base::obj().canNav()) nav.open();}
        else {if(!Base::obj().canNav()) nav.close();}
      }
    };
  };

  //text provided at type level
  template<const char** text>
  struct StaticText {
    template<typename I=Empty<Nil>>
    struct Part:I {
      using I::I;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        out.template raw<decltype(text[0]),op==Op::Printing>(text[0]);
        if(delegate) I::template print<Nav,Out,op>(nav,out,level);
      }
    };
  };

  //C atyle (const char*) text string (null terminated)
  struct Text {
    template<typename I>
    struct Part:I {
      using Base=I;
      const char* text;
      template<typename... OO>
      inline Part(const char*o,OO... oo):text(o),I(oo...) {}
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level) {
        out.template raw<decltype(text),op==Op::Printing>(text);
        if(delegate) I::template print<Nav,Out,op>(nav,out,level);
      }
    };
  };

  //api calls with path to be launched as walkers
  struct SetWalker {
    template<typename I>
    struct Part:I {
      using Base=I;
      using This=Part<I>;

      using Base::size;
      inline size_t size(PathRef ref) {return APICall::Size().walkPath(Base::obj(),ref,0);}

      using Base::enabled;
      inline bool enabled(PathRef ref) {return APICall::Enabled().walkPath(Base::obj(),ref,0);}

      using Base::enable;
      inline void enable(bool b,PathRef ref) {
        if(b) APICall::Enable<true>().walkPath(Base::obj(),ref,0);
        else APICall::Enable<false>().walkPath(Base::obj(),ref,0);
      }
    };
  };

  //functional parent print definition
  struct ParentPrint {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using Base::printMenu;
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level) {
        nav.print(out,nav.parent());//relaunch the print on the parent
      }
    };
  };

  //track if item is active
  struct IsActive {
    template<typename I>
    struct Part:I {
      using Base=I;
      bool active=false;
      inline bool isActive() const {return active;}
      using Base::activate;
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        trace(MDO<<"IsActive->activating!"<<endl);
        active=true;
        Base::activate(nav,level);
      }
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux,Idx n) {
        auto r=Base::template cmd<c,Nav>(nav,level,aux,n);
        active=nav.depth()==level;
        trace(if(!active) MDO<<"IsActive->deactivating!"<<endl);
        return r;
      }
    };
  };

};
