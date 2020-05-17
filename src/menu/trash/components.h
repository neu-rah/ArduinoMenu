/* -*- C++ -*- */
#pragma once

/********************************
Menu system common components
*/

#include "kernel/api.h"

namespace Menu {

  // alias composition blocks
  //ie: `using Test=Alias<ToggleNav::Part,WrapNav::Part,KeepSel::Part>;`
  //    ...
  //    `Item<Test::Part,Mutable::Part>,`
  template<Expr... OO>
  struct Alias {
    template<typename I>
    struct Part:Chain<OO...>::template To<I> {};
  };


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
      inline void printMenuItem(Nav& nav,Out& out,Idx level,Idx n) {
        trace(MDO<<"ParentPrint::printMenuItem @"<<n<<endl);
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
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        auto r=Base::template cmdItem<c,Nav>(nav,level,aux,n);
        active=nav.depth()==level;
        trace(if(!active) MDO<<"IsActive->deactivating!"<<endl);
        return r;
      }
    };
  };

  //call action on navigation focus change and set the value as if activated
  //(but does not change nav/activation state)
  struct ActOnUpdate {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        trace(MDO<<"ActOnUpdate::cmdItem "<<c<<endl);
        if(c!=Cmd::Enter&&c!=Cmd::Esc&&Base::obj().isActive()) {
          if(Base::template cmdItem<c,Nav>(nav,level,aux,n)) {
            trace(MDO<<"ActOnUpdate::cmdItem level:"<<level<<" focus:"<<nav.pos()<<endl);
            trace(MDO<<"updating value to:"<<Base::obj().template getValue<typename Base::ValueType>(nav.pos())<<endl);
            Base::obj().setValue(
              Base::obj().template getValue<typename Base::ValueType>(nav.pos())
            );
            Base::obj().storeSel(nav.pos());
            Base::obj().action(nav,level);
            return true;
          }
          return false;
        }
        return Base::template cmdItem<c,Nav>(nav,level,aux,n);
      }
    };
  };

  //keep navigation selection state
  template<typename I>
  class KeepSelCore:public I {
    public:
      inline void storeSel(Idx n) {sel=n;}
      inline Idx storedSel() const {return sel;}
      template<typename Nav>
      inline void restoreSel(Nav& nav) {nav.setPos(sel,nav.depth());}
      template<typename Nav>
      inline void restoreSel(Nav& nav,Idx level) {nav.setPos(sel,level);}
    protected:
      Idx sel=0;
  };

  //recall selected/active index
  struct KeepSel {
    template<typename I>
    struct Part:KeepSelCore<I> {
      using Base=KeepSelCore<I>;
      using Base::Base;
      using Base::activate;
      // inline Idx focusIndex() const {return sel;}
      // inline void keepSel(Idx n) {sel=n;}
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        trace(MDO<<"{KeepSel} User selection was #"<<sel<<" path"<<nav.operator PathRef()<<endl);
        Base::activate(nav,level);
        nav.setPos(Base::obj().storedSel(),nav.depth());
        trace(MDO<<"{KeepSel} retored selection of level "<<nav.depth()<<" to "<<nav.operator PathRef()<<endl);
      }
      template<typename Nav>
      inline void activate(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"{KeepSel} User selects #"<<n<<endl);
        Base::obj().storeSel(n);
        Base::activate(nav,level,n);
      }
      template<typename Nav>
      inline bool action(Nav& nav,Idx level) {
        trace(MDO<<"{KeepSel} action storing:"<<nav.pos()<<" active:"<<Base::obj().isActive()<<endl);
        Base::obj().storeSel(nav.pos());
        return Base::action(nav,level);
      }
    };
  };

  //track focus change due to `cmd` activity
  //and sends `changed` messages on intervinient children
  //this must be a top level to work
  struct CmdChange {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux) {
        Idx p=nav.pos();
        bool res=Base::template cmd<c,Nav>(nav,level,aux);
        if(p!=nav.pos()) {
          Base::obj().changed(p,true);
          Base::obj().changed(nav.pos(),true);
        }
        return res;
      }
    };
  };

  //call action on child activation
  struct ActOnSub {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using Base::activate;
      template<typename Nav>
      inline void activate(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"ActOnSub::activate #"<<n<<endl);
        Base::obj().action(nav,level);
        Base::activate(nav,level,n);
      }
    };
  };

  //navigation abstration
  template<typename I>
  class NavCmdAbs:public I {
    public:
      using Base=I;
      using Base::Base;
    protected:
      template<Cmd c,typename Nav>
      inline bool cmdAbs(Nav& nav,Idx level,Idx aux,Idx n) {
        _trace(MDO<<"NavCmdAbs::cmd "<<aux<<" level:"<<level<<" #"<<n<<" mode:"<<nav.mode()<<" nav.level:"<<nav.depth()<<" canNav:"<<Base::canNav(n)<<endl);
        bool r=Base::template cmd<c,Nav>(nav,level,aux,n);
        switch(c) {
          case Cmd::Up:
            if(n<Base::size()-1) {
              nav.setPos(n+1);
              return true;
            }
            break;
          case Cmd::Down:
            if(n>0) {
              nav.setPos(n-1);
              return true;
            }
            break;
          case Cmd::Enter:
            nav.activate();
            return true;
          default:break;
        }
        return r;
      }
  };

  //enumeration nav, no check for num. field mode
  struct EnumNav {
    template<typename I>
    struct Part:NavCmdAbs<I> {
      using Base=NavCmdAbs<I>;
      using Base::Base;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        _trace(MDO<<"EnumNav::cmd "<<aux<<" level:"<<level<<" #"<<n<<" mode:"<<nav.mode()<<" nav.level:"<<nav.depth()<<" canNav:"<<Base::canNav(n)<<endl);
        return Base::template cmdAbs<c,Nav>(nav,level,aux,n);
      }
    };
  };

  //default navigation
  struct DefaultNav {
    template<typename I>
    struct Part:NavCmdAbs<I> {
      using Base=NavCmdAbs<I>;
      using Base::Base;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        _trace(MDO<<"DefaultNav::cmdItem "<<aux<<" level:"<<level<<" #"<<n<<" mode:"<<nav.mode()<<" nav.level:"<<nav.depth()<<" canNav:"<<Base::canNav(n)<<endl);
        bool r=Base::template cmdItem<c,Nav>(nav,level,aux,n);
        if(nav.mode()==Mode::Normal||(Base::obj().canNav()&&nav.mode()!=Mode::Normal))
          return r||Base::template cmdAbs<c,Nav>(nav,level,aux,n);
        return r;
      }
    };
  };

  //circular navigation
  struct WrapNav {
    template<typename I>
    struct Part:I {
      using Base=I;
      using Base::Base;
      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        trace(MDO<<"WrapNav::cmdItem "<<aux<<" level:"<<level<<" #"<<n<<" mode:"<<nav.mode()<<endl);
        bool r=Base::template cmdItem<c,Nav>(nav,level,aux,n);
        if(nav.mode()==Mode::Normal) switch(c) {
          case Cmd::Up:
            n++;
            if(n>=Base::obj().sizeItem(n)) n=0;
            nav.setPos(n);
            return true;
          case Cmd::Down:
            n--;
            if(n<0) n=Base::obj().sizeItem(n)-1;
            nav.setPos(n);
            return true;
          case Cmd::Enter:
            nav.activate();
          case Cmd::Esc: nav.close();
          default:break;
        }
        return r;
      }
    };
  };

  struct ToggleNav {
    template<typename I>
    struct Part:KeepSelCore<I> {
      using Base=KeepSelCore<I>;
      using Base::Base;
      using Base::cmd;
      template<typename Nav>
      bool toggle(Nav& nav,Idx level) {
        int n=Base::obj().storedSel()+1;
        if(n>=Base::size()) n=0;
        Base::obj().storeSel(n);
        nav.setPos(n,level+1);
        Base::obj().setValue(
          Base::template getValue<typename Base::ValueType>(n)
        );
        Base::obj().action(nav,level);
        return false;
      }
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux) {
        trace(MDO<<"Toggle::cmd "<<aux<<" level:"<<level<<" mode:"<<nav.mode()<<" size:"<<Base::size()<<endl);
        if(c==Cmd::Enter) {
          Base::template cmd<c,Nav>(nav,level,aux);
          return true;///toggle(nav,level);
        }
        return Base::template cmd<c,Nav>(nav,level,aux);
      }
      using Base::activate;
      template<typename Nav>
      inline void activate(Nav& nav,Idx level) {
        trace(MDO<<"Toggle::activate"<<endl);
        toggle(nav,level);
      }
    };
  };
};
