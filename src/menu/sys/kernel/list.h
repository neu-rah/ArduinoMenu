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

    template<typename T>
    inline T getValue(Idx n) const
      {return n?tail.template getValue<T>(n-1):F::template getValue<T>();}

    using Base::activate;
    template<typename Nav>
    inline void activate(Nav& nav,Idx level,Idx n) {
      trace(MDO<<"Pair::activate n:"<<n<<endl);
      if(n) tail.template activate<Nav>(nav,level,n-1);
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
    inline constexpr size_t size(Idx n) const {return n?tail.size(n-1):F::size();}
    inline constexpr size_t count() const {return tail.count()+1;}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx level,Idx aux,Idx n) {
      trace(MDO<<"Pair::cmd "<<c<<" aux:"<<aux<<" n:"<<n<<endl);
      if(n) return tail.template cmd<c,Nav>(nav,level,aux,n-1);
      return F::template cmd<c,Nav>(nav,level,aux);
    }

    inline void changed(Idx i,bool o) {
      if (i) tail.changed(i-1,o);
      else F::changed(o);
    }

    using Base::enabled;
    inline bool enabled(Idx n) {
      trace(MDO<<"Pair::enabled n:"<<n<<endl);
      return n?tail.enabled(n-1):F::enabled();
    }
    using Base::enable;
    inline void enable(bool b,Idx n) {
      trace(MDO<<"Pair::enable("<<b<<","<<n<<")"<<endl);
      return n?tail.enable(b,n-1):F::enable(b);
    }

    template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
    inline void print(Nav& nav,Out& out,Idx level,Idx n) {
      trace(MDO<<"Pair::print "<<n<<endl);
      if(n) tail.template print<Nav,Out,op,delegate>(nav,out,level,n-1);
      else F::template print<Nav,Out,op,delegate>(nav,out,level+1);
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,Idx level) {
      out.template printMenu<typename F::Type,Nav,op>
        (F::obj(),nav,level,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,Idx level,Idx n) {
      if (n) tail.template printMenu<Nav,Out,op>(nav,out,level,n);
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

  //static poly-morphic menu
  template<typename Title,typename Body>
  struct StaticMenu {
    template<typename I>
    struct Part:SetWalker::template Part<I> {
      using Base=typename SetWalker::template Part<I>;
      Title title;
      Body body;
      using This=StaticMenu<Title,Body>;
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
        trace(MDO<<"StaticMenu::walkPath"<<endl);
        return body.walkPath(api,ref,level,n);
      }

      template<typename T>
      inline T getValue(Idx n) const {return body.template getValue<T>(n);}

      using Base::size;
      inline constexpr size_t size() const {return body.count();}
      inline constexpr size_t size(Idx n) const {return body.size(n);}
      using Base::enabled;
      inline bool enabled(Idx n) {
        _trace(MDO<<"StaticMenu::enabled "<<n<<endl);
        return body.enabled(n);}
      using Base::enable;
      inline void enable(bool b,Idx n) {
        trace(MDO<<"StaticMenu::enable("<<b<<","<<n<<")"<<endl);
        body.enable(b,n);}
      inline static constexpr bool canNav() {return true;}
      template<typename Nav>
      inline static void activate(Nav& nav,Idx) {
        trace(MDO<<"StaticMenu::activate!"<<endl);
        nav.open();
      }
      template<typename Nav>
      inline void activate(Nav& nav,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::activate n:"<<n<<endl);
        body.template activate<Nav>(nav,level,n);
      }

      using Base::changed;
      inline void changed(Idx i,bool o) {body.changed(i,o);}

      using Base::cmd;
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux) {
        Idx p=nav.pos();
        bool res=Base::template cmd<c,Nav>(nav,level,aux);
        if(p!=nav.pos()) {
          changed(p,true);
          changed(nav.pos(),true);
        }
        return res;
      }
      template<Cmd c,typename Nav>
      inline bool cmd(Nav& nav,Idx level,Idx aux,Idx n) {
        trace(MDO<<"StaticMenu::cmd to item:"<<n<<endl);
        return body.template cmd<c,Nav>(nav,level,aux,n);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level) {
        trace(MDO<<"StaticMenu::printMenu"<<endl);
        out.template printMenu<typename Base::Type,Nav,op>
          (Base::obj(),nav,out.fullDraw()||!out.isSame(&Base::obj()),level);
      }

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::printMenu "<<n<<endl);
        // body.template printMenu<Nav,Out,op,delegate>(nav,out,level,n);
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
      inline void print(Nav& nav,Out& out,Idx level) {
        trace(MDO<<"StaticMenu::print "<<op<<endl);
        title.template print<Nav,Out,op,delegate>(nav,out,level);
      }

      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,Idx n) {
        trace(MDO<<"StaticMenu::print "<<n<<endl);
        body.template print<Nav,Out,op,delegate>(nav,out,level,n);
      }
    };
  };
};
