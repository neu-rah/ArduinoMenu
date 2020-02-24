/* -*- C++ -*- */
#pragma once

#include "staticIO.h"

namespace Menu {
  struct IOut {
    virtual inline void nl(bool toPrint=true)=0;
    virtual void raw(const char* o,bool toPrint=true)=0;
    virtual void printMenu(IItem& it,INav& nav,bool fullPrint,Op=Op::Printing)=0;
    virtual void printTitle(IItem& it,INav& nav,Op op=Op::Printing)=0;
    virtual void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,Op op=Op::Printing)=0;
    virtual Idx freeY() const=0;
    virtual bool partialDraw() const=0;
    virtual bool isSame(void*) const=0;

    inline bool fullDraw() {return !partialDraw();}

    template<typename It,typename Nav,Op op=Op::Printing>
    void printTitle(It& it,Nav& nav) {printTitle(it,nav,op);}

    template<typename It,typename Nav,Op op=Op::Printing>
    inline void printMenu(It& it,Nav& nav,bool fullPrint) {printMenu(it,nav,fullPrint,op);}

    // template<typename Nav,typename Out,Op op=Op::Printing,Tag role=Tag::Raw>
    // inline void print(Nav& nav,Out& out)
    template<typename It,typename Nav,Op op=Op::Printing>
    void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      printItem(it,nav,n,s,e,m,op);
    }

    template<typename T,bool toPrint=true>
    inline void raw(T o) {raw(o,toPrint);}

  };

  template<Expr...  O>
  struct MenuOut:IOut,Chain<O...,Void>::template To<Obj<MenuOut<O...>>> {
    using Base=typename Chain<O...,Void>::template To<Obj<MenuOut<O...>>>;
    using This=MenuOut<O...>;
    using Base::Base;
    using Base::nl;
    using Base::raw;
    inline void nl(bool toPrint=true) override {
      toPrint?Base::template nl<true>():Base::template nl<false>();
    }
    inline void raw(const char* o,bool toPrint=true) override {
      if (toPrint) Base::template raw<decltype(o),true>(o);
      else Base::template raw<decltype(o),false>(o);
    }
    void printMenu(IItem& it,INav& nav,bool fullPrint, Op op=Op::Printing) override;
    void printTitle(IItem& it,INav& nav,Op op=Op::Printing) override;
    void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,Op op=Op::Printing) override;
    Idx freeY() const override {return Base::freeY();}
    bool partialDraw() const {return Base::partialDraw();}
    bool isSame(void* o) const {return Base::isSame(o);}
  };
}
