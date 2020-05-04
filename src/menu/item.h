/* -*- C++ -*- */
#pragma once

#include "staticItem.h"

namespace Menu {

  struct IAPI {
    virtual APIRes call(IItem& at,Idx n=0)=0;
    virtual bool chk(IItem&,PathRef ref)=0;
    #ifdef MENU_DEBUG
      virtual const char* named()=0;
    #endif
};

  template<typename A>
  struct IAPICall:IAPI,A {
    using A::A;
    IAPICall(A& a):A(a){}
    using A::call;
    APIRes call(IItem& at,Idx n=0) override {
      return A::call(at,n);
    }
    bool chk(IItem& at,PathRef ref) override {return A::chk(at,ref);}
    #ifdef MENU_DEBUG
      const char* named() override {return A::named();}
    #endif
  };

  struct IItem {
    virtual void printTitle(INav& nav,IOut& out,bool fullPrint,Op op=Op::Printing)=0;
    virtual bool changed() const=0;
    virtual void changed(bool o)=0;
    virtual bool cmd(Cmd,INav&,Idx=0)=0;
    virtual size_t size() const=0;
    virtual bool enabled() const=0;
    virtual ActRes activate()=0;
    virtual void printMenu(INav& nav,IOut& out,Op op=Op::Printing)=0;
    virtual void enable(bool b)=0;
    virtual void printItems(INav&,IOut&,bool fullPrint,Idx=0,Idx=0,Op op=Op::Printing)=0;
    virtual void print(INav&,IOut&,Op op)=0;
    virtual bool parentPrint()=0;
    virtual bool parentPrint(Idx)=0;

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printTitle(Nav& nav,Out& out,bool fullPrint) {printTitle(nav,out,fullPrint,op);}
    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx n=0) {return cmd(c,nav,n);}

    template<typename A>
    APIRes walkPath(A& api,PathRef ref) {
      auto a=IAPICall<A>(api);
      return iWalkPath(a,ref);
    }
    APIRes walkPath(IAPI& api,PathRef ref) {
      return iWalkPath(api,ref);
    }
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out) {printMenu(nav,out,op);}
    inline bool enabled(PathRef ref) {
      auto api=IAPICall<APICall::Enabled>();
      return iWalkPath(api,ref);
    }
    inline size_t size(PathRef ref) {
      trace(MDO<<"IItem::size"<<endl);
      auto f=IAPICall<APICall::Size>();
      iWalkPath(f,ref);
    }
    inline void enable(bool b,PathRef ref) {
      auto en=IAPICall<APICall::Enable<true>>();
      auto dis=IAPICall<APICall::Enable<false>>();
      if(b) iWalkPath(en,ref);
      else iWalkPath(dis,ref);
    }
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0)
      {printItems(nav,out,fullPrint,idx,top,op);}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void print(Nav& nav,Out& out)
      {print(nav,out,op);}
  protected:
    virtual APIRes iWalkPath(IAPI& api,PathRef ref)=0;
  };

  template<Expr... I>
  struct Prompt:IItem,Chain<I...,Empty>::template To<Obj<Prompt<I...>>> {
    using Base=typename Chain<I...,Empty>::template To<Obj<Prompt<I...>>>;
    using This=Prompt<I...>;
    using Base::Base;
    using Base::printItems;
    using Base::printMenu;
    using Base::print;
    using Base::cmd;
    using IItem::size;
    using IItem::enabled;
    using IItem::walkPath;
    using IItem::enable;
    inline void printTitle(INav& nav,IOut& out,bool fullPrint,Op op=Op::Printing) override;
    inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing) override;
    inline size_t size() const override {return Base::size();}
    inline bool enabled() const override {return Base::enabled();}
    inline void enable(bool o) override {Base::enable(o);}
    inline bool changed() const override {return Base::changed();}
    inline void changed(bool o) override {Base::changed(o);}
    inline bool cmd(Cmd,INav&,Idx) override;
    inline ActRes activate() override {return Base::activate();}
    inline void printItems(INav& nav,IOut& out,bool fullPrint,Idx idx=0,Idx top=0,Op op=Op::Printing) override;
    inline void print(INav& nav,IOut& out,Op op) override;
    bool parentPrint() override {return Base::parentPrint();}
    bool parentPrint(Idx n) override {return Base::parentPrint(n);}
  protected:
    APIRes iWalkPath(IAPI& api,PathRef ref) override {
      trace(MDO<<"Prompt::iWalkPath "<<api.named()<<endl);
      return Base::walkPath(api,ref);}
  };
};
