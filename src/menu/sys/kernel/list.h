/* -*- C++ -*- */
#pragma once

/********************************
Options list base and components
*/

#include "api.h"

namespace Menu {

  //chain of elements, this is used as a list node
  template<typename F,typename S=Item<Empty>>
  struct Pair:F {
    using Base=F;
    using This=Pair<F,S>;
    using F::F;
    S tail;
    using F::changed;

    inline constexpr bool canNav(Idx n) {
      return n?tail.canNav(n-1):F::canNav();
    }

    template<typename T>
    inline T getValue(Idx n) const
      {return n?tail.template getValue<T>(n-1):F::template getValue<T>();}

    using Base::activate;
    template<typename Nav>
    inline void activateItem(Nav& nav,Idx level,Idx n) {
      trace(MDO<<"Pair::activate n:"<<n<<endl);
      if(n) tail.template activateItem<Nav>(nav,level,n-1);
      else F::template activate<Nav>(nav,level);
    }

    template<typename A,Idx i>
    inline APIRes walkId(const A& api,Idx level) {
      return F::id(i)?api.call(*this,level):tail.template walkId<A,i>(api,level);
    }
    template<typename A>
    inline APIRes walkId(const A& api,Idx i,Idx level) {
      return F::id(i)?
        api.call(*this,level):
        tail.walkId(api,i,level);
    }

    template<typename A>
    APIRes walkPath(const A& api,PathRef ref,Idx level,Idx n) {
      trace(MDO<<"Pair::walkPath"<<endl);
      return n?
        tail.walkPath(api,ref,level,n-1):
        api.step(F::obj(),ref,level);
    }

    using Base::size;
    inline constexpr size_t sizeItem(Idx n) const {return n?tail.sizeItem(n-1):F::size();}
    inline constexpr size_t count() const {return tail.count()+1;}

    template<Cmd c,typename Nav>
    inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
      trace(MDO<<"Pair::cmdItem "<<c<<" aux:"<<aux<<" n:"<<n<<endl);
      if(n) return tail.template cmdItem<c,Nav>(nav,level,aux,n-1);
      return F::template cmd<c,Nav>(nav,level,aux);
    }

    inline void changed(Idx i,bool o) {
      if (i) tail.changed(i-1,o);
      else F::changed(o);
    }

    using Base::enabled;
    inline bool enabledItem(Idx n) {
      trace(MDO<<"Pair::enabled n:"<<n<<endl);
      return n?tail.enabledItem(n-1):F::enabled();
    }
    using Base::enable;
    inline void enableItem(bool b,Idx n) {
      trace(MDO<<"Pair::enable("<<b<<","<<n<<")"<<endl);
      return n?tail.enableItem(b,n-1):F::enable(b);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
    inline void printItem(Nav& nav,Out& out,Idx level,Idx n) {
      trace(MDO<<"Pair::print "<<n<<endl);
      if(n) tail.template printItem<Nav,Out,op,delegate>(nav,out,level,n-1);
      else F::template print<Nav,Out,op,delegate>(nav,out,level+1,nav.selected(n,level));
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,Idx level) {
      out.template printMenu<typename F::Type,Nav,op>
        (F::obj(),nav,level,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenuItem(Nav& nav,Out& out,Idx level,Idx n) {
      if (n) tail.template printMenuItem<Nav,Out,op>(nav,out,level,n);
      else  tail.template printMenu<Nav,Out,op>(nav,out,level);
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    void printItems(Nav& nav,Out& out,Idx level,bool fullPrint,Idx idx=0,Idx top=0) {
      trace(MDO<<"Pair<...>::printItems level:"<<level<<" out:@"<<(unsigned long)&out<<" out.fullDraw:"<<out.fullDraw()<<endl);
      if (op==Op::Printing&&(fullPrint||out.fullDraw()||F::changed()))
        out.template printItem<typename F::Type,Nav,op>(F::obj(),nav,level,idx,nav.selected(idx,level),F::enabled(),nav.mode());
      else out.template printItem<
          typename F::Type,
          Nav,op==Op::Printing?Op::Measure:op
        >(F::obj(),nav,level,idx,nav.selected(idx),F::enabled(),nav.mode());
      tail.template printItems<Nav,Out,op>(nav,out,level,fullPrint,idx+1,top);
    }
  };

  //a list of chained nodes
  template<typename O,typename... OO> struct StaticData:Pair<O,StaticData<OO...>> {};
  template<typename O>                struct StaticData<O>:Pair<O> {};

  //static poly-morphic menu core
  //a core is a full component but it has some functionality that should not be on top of the composition
  //in this case the `cmd` handler that is delivered to the bosy and therefore does NOT follow the composution chain
  template<typename Title,typename Body>
  struct StaticMenuCore {
    template<typename I>
    struct Part:I {
      using Base=I;
      Title title;
      Body body;
      using This=Part<I>;
      using Base::Base;

      template<typename A,Idx i>
      inline APIRes walkId(const A& api,Idx level) {
        return Base::id(i)?
          api.call(*this,level):
          body.template walkId<A,i>(api,level);
      }
      template<typename A>
      inline APIRes walkId(const A& api,Idx i,Idx level) {
        return Base::id(i)?
          api.call(*this,level):
          body.walkId(api,i,level);
      }

      template<typename A>
      APIRes walkPath(const A& api,PathRef ref,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::walkPath "<<api.named()<<" "<<ref<<endl);
        return body.walkPath(api,ref,level,n);
      }

      template<typename T>
      inline T getValue(Idx n) const {return body.template getValue<T>(n);}

      using Base::size;
      inline constexpr size_t size() const {return body.count();}
      inline constexpr size_t sizeItem(Idx n) const {return body.sizeItem(n);}
      using Base::enabled;
      inline bool enabledItem(Idx n) {
        trace(MDO<<"StaticMenu::enabled "<<n<<endl);
        return body.enabledItem(n);}
      using Base::enable;
      inline void enableItem(bool b,Idx n) {
        trace(MDO<<"StaticMenu::enable("<<b<<","<<n<<")"<<endl);
        body.enableItem(b,n);}
      inline static constexpr bool canNav() {return true;}
      inline constexpr bool canNav(Idx n) {return body.canNav(n);}
      template<typename Nav>
      inline static void activate(Nav& nav,Idx) {
        trace(MDO<<"StaticMenu::activate!"<<endl);
        nav.open();
      }
      template<typename Nav>
      inline void activateItem(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::activate n:"<<n<<endl);
        body.template activateItem<Nav>(nav,level,n);
      }

      using Base::changed;
      inline void changed(Idx i,bool o) {body.changed(i,o);}

      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmdItem(Nav& nav,Idx level,Idx aux,Idx n) {
        trace(MDO<<"StaticMenu::cmdItem to item:"<<n<<endl);
        return body.template cmdItem<c,Nav>(nav,level,aux,n);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level) {
        trace(MDO<<"StaticMenu::printMenu"<<endl);
        out.template printMenu<typename Base::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()),level);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenuItem(Nav& nav,Out& out,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::printMenuItem "<<n<<endl);
        // body.template printMenuItem<Nav,Out,op,delegate>(nav,out,level,n);
        //TODO: make this pure function even if slow
        out.template printMenu<typename Base::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()),level);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printItems(Nav& nav,Out& out,Idx level,bool fullPrint,Idx idx=0,Idx top=0) {
        trace(MDO<<"StaticMenu::printItems"<<endl);
        body.template printItems<Nav,Out,op>(nav,out,level,fullPrint,idx,top);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printTitle(Nav& nav,Out& out,Idx level,bool fullPrint) {
        trace(MDO<<"StaticMenu::printTitle "<<op<<" fullDraw:"<<out.fullDraw()<<" changed:"<<title.changed()<<" out:"<<((long)&out)<<endl);
        if (op==Op::ClearChanges) title.changed(false);
        else {
          if (op==Op::Printing&&(fullPrint||out.fullDraw()||title.changed()))
            out.template printTitle<typename Base::Type,Nav,op>(Base::obj(),nav,level);
          else
            out.template printTitle<typename Base::Type,Nav,op==Op::Printing?Op::Measure:op>(Base::obj(),nav,level);
        }
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,bool selected) {
        trace(MDO<<"StaticMenu::print "<<op<<endl);
        title.template print<Nav,Out,op,delegate>(nav,out,level,selected);
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void printItem(Nav& nav,Out& out,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::print "<<n<<endl);
        body.template printItem<Nav,Out,op,delegate>(nav,out,level,n);
      }
    };
  };

  template<typename Title,typename Body>
  using StaticMenu=Alias<
    CmdChange::Part,
    DefaultNav::Part,
    SetWalker::Part,
    StaticMenuCore<Title,Body>::template Part
  >;

};
