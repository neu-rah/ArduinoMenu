/* -*- C++ -*- */
#pragma once

/**************
the dynamic/virtual interface for items
*/

#include "staticItem.h"

namespace Menu {

  struct IAPI {
    virtual APIRes step(IItem& at,PathRef ref,Idx level=0) const=0;
    virtual APIRes call(IItem& at,Idx level)=0;
    virtual APIRes call(IItem& at,Idx level,Idx n)=0;
    virtual APIRes walkPath(IItem& at,PathRef ref,Idx level) const=0;
    #ifdef MENU_DEBUG
      virtual const char* named()=0;
    #endif
};

  template<typename A>
  struct IAPICall:IAPI,A {
    using A::A;
    IAPICall(const A& a):A(a){}
    using A::call;
    inline APIRes step(IItem& at,PathRef ref,Idx level=0) const {
      return A::step(at,ref,level);
    }
    APIRes call(IItem& at,Idx level) override {
      return A::call(at,level);
    }
    APIRes call(IItem& at,Idx level,Idx n) override {
      return A::call(at,level,n);
    }
    APIRes walkPath(IItem& at,PathRef ref,Idx level) const override {
      A::walkPath(at,ref,level);
    }
    #ifdef MENU_DEBUG
      const char* named() override {return A::named();}
    #endif
  };

  template<typename A>
  struct IAPICall<IAPICall<A>> {};

  struct IItem {
    virtual void printTitle(INav& nav,IOut& out,Idx level,bool fullPrint,Op op=Op::Printing)=0;
    virtual bool changed() const=0;
    virtual void changed(bool o)=0;
    virtual bool cmd(Cmd,INav&,Idx level,Idx aux)=0;
    virtual bool cmd(Cmd,INav&,Idx level,Idx aux,Idx)=0;
    virtual size_t size() const=0;
    virtual size_t size(Idx) const=0;
    virtual bool enabled() const=0;
    virtual bool enabled(Idx) const=0;
    virtual void printMenu(INav& nav,IOut& out,Idx level,Op op=Op::Printing)=0;
    virtual void printMenu(INav& nav,IOut& out,Idx level,Idx n,Op op=Op::Printing)=0;
    virtual void enable(bool b)=0;
    virtual void enable(bool b,Idx)=0;
    virtual void printItems(INav&,IOut&,bool fullPrint,Idx=0,Idx=0,Op op=Op::Printing)=0;
    virtual void print(INav&,IOut&,Idx level,Op op)=0;
    inline IItem& obj() {return *this;}

  // protected:
    virtual APIRes iWalkPath(const IAPI& api,PathRef ref,Idx level)=0;
    virtual APIRes iWalkPath(const IAPI& api,PathRef ref,Idx level,Idx)=0;
    virtual void iActivate(INav&,Idx)=0;
    virtual void iActivate(INav&,Idx,Idx)=0;

  // public:
    inline size_t size(PathRef ref) {return APICall::Size().walkPath(obj(),ref,0);}

    inline bool enabled(PathRef ref) {return APICall::Enabled().walkPath(obj(),ref,0);}

    inline void enable(bool b,PathRef ref) {
      if(b) APICall::Enable<true>().walkPath(obj(),ref,0);
      else APICall::Enable<false>().walkPath(obj(),ref,0);
    }

    template<typename Nav>
    inline void activate(Nav& nav,Idx level) {iActivate(nav,level);}
    template<typename Nav>
    inline void activate(Nav& nav,Idx level,Idx n) {iActivate(nav,level,n);}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printTitle(Nav& nav,Out& out,Idx level,bool fullPrint)
      {printTitle(nav,out,level,fullPrint,op);}
    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx level,Idx aux) {return cmd(c,nav,level,aux);}
    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,Idx level,Idx aux,Idx n) {return cmd(c,nav,level,aux,n);}

    template<typename A>
    APIRes walkPath(const A& api,PathRef ref,Idx level) {
      return iWalkPath(api,ref,level);
    }
    template<typename A>
    APIRes walkPath(const A& api,PathRef ref,Idx level,Idx n) {
      return iWalkPath(IAPICall<A>(api),ref,level,n);
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0)
      {printItems(nav,out,fullPrint,idx,top,op);}

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void print(Nav& nav,Out& out,Idx level)
      {print(nav,out,level,op);}

      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level)
        {printMenu(nav,out,level,op);}
      template<typename Nav,typename Out,Op op=Op::Printing>
      inline void printMenu(Nav& nav,Out& out,Idx level,Idx n)
        {printMenu(nav,out,level,n,op);}
  };

  template<Expr... I>
  struct Prompt:IItem,Chain<I...,Empty>::template To<Obj<Prompt<I...>>> {
    using Base=typename Chain<I...,Empty>::template To<Obj<Prompt<I...>>>;
    using This=Prompt<I...>;
    using Base::Base;
    using Base::printItems;
    using IItem::printMenu;
    using Base::print;
    using Base::cmd;
    using IItem::size;
    using IItem::enabled;
    using IItem::walkPath;
    using IItem::enable;
    using IItem::obj;
    using Base::activate;
    inline void printTitle(INav& nav,IOut& out,Idx level,bool fullPrint,Op op=Op::Printing) override;
    inline void printMenu(INav& nav,IOut& out,Idx level,Op op=Op::Printing) override;
    inline void printMenu(INav& nav,IOut& out,Idx level,Idx n,Op op=Op::Printing) override;
    inline size_t size() const override {return Base::size();}
    inline size_t size(Idx n) const override {return Base::size(n);}
    inline bool enabled() const override {return Base::enabled();}
    inline bool enabled(Idx n) const override {return Base::enabled(n);}
    inline void enable(bool o) override {Base::enable(o);}
    inline void enable(bool o,Idx n) override {Base::enable(o,n);}
    inline bool changed() const override {return Base::changed();}
    inline void changed(bool o) override {Base::changed(o);}
    inline bool cmd(Cmd,INav&,Idx level,Idx) override;
    inline bool cmd(Cmd,INav&,Idx level,Idx,Idx) override;
    inline void printItems(INav& nav,IOut& out,bool fullPrint,Idx idx=0,Idx top=0,Op op=Op::Printing) override;
    inline void print(INav& nav,IOut& out,Idx level,Op op) override;
  // protected:
    APIRes iWalkPath(const IAPI& api,PathRef ref,Idx level) override {
      trace(MDO<<"Prompt::iWalkPath "<<api.named()<<" "<<ref<<endl);
      return Base::template walkPath<decltype(api)>(api,ref,level);
    }
    APIRes iWalkPath(const IAPI& api,PathRef ref,Idx level,Idx n) override {
      trace(MDO<<"Prompt::iWalkPath "<<api.named()<<" "<<ref<<" "<<n<<endl);
      return APIRes(Base::walkPath(api,ref,level,n));
    }
    inline void iActivate(INav& nav,Idx level) override {Base::activate(nav,level);}
    inline void iActivate(INav& nav,Idx level,Idx n) override {Base::activate(nav,level,n);}
  };
};
