/* -*- C++ -*- */
#pragma once

#include "menuIO.h"

struct IOut {
  virtual inline void nl(bool toPrint=true)=0;
  virtual void raw(const char* o,bool toPrint=true)=0;
  virtual void printMenu(IItem& it,INav& nav, Op=Op::Printing)=0;
  virtual void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Op op=Op::Printing,bool toPrint=true)=0;
  virtual Idx freeY() const=0;

  template<typename It,typename Nav,Op op=Op::Printing>
  inline void printMenu(It& it,Nav& nav) {printMenu(it,nav,op);}

  // template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
  // inline void print(Nav& nav,Out& out)
  template<typename It,typename Nav,Op op=Op::Printing,bool toPrint=true>
  void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    printItem(it,nav,n,s,e,m,op,toPrint);
  }
};

template<Expr...  O>
struct MenuOut:IOut,Chain<O...,Void>::template To<Obj<MenuOut<O...>>> {
  using Base=typename Chain<O...,Void>::template To<Obj<MenuOut<O...>>>;
  using This=MenuOut<O...>;
  using Base::Base;
  inline void nl(bool toPrint=true) override {
    toPrint?Base::template nl<true>():Base::template nl<false>();
  }
  inline void raw(const char* o,bool toPrint=true) override {Base::template raw<decltype(o,toPrint)>(o);}
  void printMenu(IItem& it,INav& nav, Op op=Op::Printing) override;
  void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal,Op op=Op::Printing,bool toPrint=true) override;
  Idx freeY() const override {return Base::freeY();}
};
