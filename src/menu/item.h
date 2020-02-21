/* -*- C++ -*- */
#pragma once

#include "staticItem.h"

namespace Menu {
  struct IItem {
    virtual inline size_t size(PathRef=self)=0;
    virtual inline size_t canNav(PathRef=self)=0;
    virtual inline bool activate(PathRef ref=self)=0;
    virtual inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing,PathRef=self)=0;
    virtual inline void printTitle(INav& nav,IOut& out,bool fullPrint,Op op=Op::Printing)=0;
    virtual inline void printItems(INav&,IOut&,bool fullPrint,Idx=0,Idx=0,PathRef=self,Op op=Op::Printing)=0;
    virtual inline void print(INav&,IOut&,Op op,PathRef=self)=0;
    virtual inline bool enabled(PathRef=self)=0;
    virtual inline void enable(bool,PathRef=self)=0;
    virtual inline bool changed() const=0;
    virtual inline void changed(bool o)=0;
    virtual inline bool parentPrint(PathRef ref=self)=0;
    virtual inline bool cmd(Cmd,INav&,PathRef=self)=0;

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self) {printMenu(nav,out,op,ref);}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printTitle(Nav& nav,Out& out,bool fullPrint) {printTitle(nav,out,fullPrint,op);}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printItems(Nav& nav,Out& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self)
      {printItems(nav,out,fullPrint,idx,top,ref,op);}
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void print(Nav& nav,Out& out,PathRef ref=self) {print(nav,out,op,ref);}

    // template<typename Nav,typename Out,Roles role=Roles::Raw>
    // inline static void measure(Nav& nav,Out& out,PathRef ref=self) {print(nav,out,ref,Op::Measure);}

    template<Cmd c,typename Nav>
    inline bool cmd(Nav& nav,PathRef ref=self) {return cmd(c,nav,ref);}
  };

  template<Expr... I>
  struct Prompt:IItem,Chain<Mutable::Part,I...,Empty>::template To<Obj<Prompt<I...>>> {
    using Base=typename Chain<Mutable::Part,I...,Empty>::template To<Obj<Prompt<I...>>>;
    using This=Prompt<I...>;
    using Base::Base;
    using Base::printItems;
    using Base::printMenu;
    using Base::print;
    using Base::cmd;
    inline size_t size(PathRef ref=self) override {return Base::size(ref);}
    inline size_t canNav(PathRef ref=self) override {return Base::canNav(ref);}
    inline bool activate(PathRef ref=self) override {return Base::activate(ref);}
    inline void printMenu(INav& nav,IOut& out,Op op=Op::Printing,PathRef ref=self) override;
    inline void printTitle(INav& nav,IOut& out,bool fullPrint,Op op=Op::Printing) override;
    inline void printItems(INav& nav,IOut& out,bool fullPrint,Idx idx=0,Idx top=0,PathRef ref=self,Op op=Op::Printing) override;
    inline void print(INav& nav,IOut& out,Op op,PathRef ref=self) override;
    inline bool enabled(PathRef ref=self) override {return Base::enabled(ref);}
    inline void enable(bool o,PathRef ref=self) override {Base::enable(o,ref);}
    inline bool changed() const override {return Base::changed();}
    inline void changed(bool o) override {Base::changed(o);}
    inline bool parentPrint(PathRef ref=self) override {return Base::parentPrint(ref);}
    inline bool cmd(Cmd,INav&,PathRef=self) override;
  };
};
