/* -*- C++ -*- */
#pragma once

#include "staticIO.h"

namespace Menu {
  struct IOut {
    virtual inline void nl(bool toPrint=true)=0;
    virtual void printMenu(IItem& it,INav& nav,bool fullPrint,Op=Op::Printing)=0;
    virtual void printTitle(IItem& it,INav& nav,Op op=Op::Printing)=0;
    virtual void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,Op op=Op::Printing)=0;
    virtual Idx freeY() const=0;
    virtual bool partialDraw() const=0;
    virtual bool isSame(void*) const=0;
    virtual void fmt(Tag r,bool io,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,bool toPrint=true)=0;
    virtual void raw(const char*,bool)=0;
    virtual void raw(int,bool)=0;

    inline bool fullDraw() {return !partialDraw();}

    template<typename It,typename Nav,Op op=Op::Printing>
    void printTitle(It& it,Nav& nav) {printTitle(it,nav,op);}

    template<typename It,typename Nav,Op op=Op::Printing>
    inline void printMenu(It& it,Nav& nav,bool fullPrint) {printMenu(it,nav,fullPrint,op);}

    template<typename It,typename Nav,Op op=Op::Printing>
    void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      printItem(it,nav,n,s,e,m,op);
    }

    template<typename T,bool toPrint=true>
    inline void raw(T o) {raw(o,toPrint);}

    template<Tag r,bool io,bool toPrint=true>
    inline void fmt(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      fmt(r,io,n,s,e,m,toPrint);
    }

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
    inline void raw(int o,bool toPrint=true) override {
      if (toPrint) Base::template raw<decltype(o),true>(o);
      else Base::template raw<decltype(o),false>(o);
    }
    void printMenu(IItem& it,INav& nav,bool fullPrint, Op op=Op::Printing) override;
    void printTitle(IItem& it,INav& nav,Op op=Op::Printing) override;
    void printItem(IItem& it,INav& nav,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,Op op=Op::Printing) override;
    Idx freeY() const override {return Base::freeY();}
    bool partialDraw() const override {return Base::partialDraw();}
    bool isSame(void* o) const override {return Base::isSame(o);}
    template<bool io,bool toPrint>
    void fmt(Tag r,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      switch(r) {
        case Tag::Panel:  Base::template fmt<Tag::Panel,io,toPrint>(n,s,e,m);break;
        case Tag::Menu:   Base::template fmt<Tag::Menu,io,toPrint>(n,s,e,m);break;
        case Tag::Title:  Base::template fmt<Tag::Title,io,toPrint>(n,s,e,m);break;
        case Tag::Body:   Base::template fmt<Tag::Body,io,toPrint>(n,s,e,m);break;
        case Tag::Item:   Base::template fmt<Tag::Item,io,toPrint>(n,s,e,m);break;
        case Tag::Index:  Base::template fmt<Tag::Index,io,toPrint>(n,s,e,m);break;
        case Tag::Cursor: Base::template fmt<Tag::Cursor,io,toPrint>(n,s,e,m);break;
        case Tag::Name:   Base::template fmt<Tag::Name,io,toPrint>(n,s,e,m);break;
        case Tag::Mode:   Base::template fmt<Tag::Mode,io,toPrint>(n,s,e,m);break;
        case Tag::Value:  Base::template fmt<Tag::Value,io,toPrint>(n,s,e,m);break;
        case Tag::Unit:   Base::template fmt<Tag::Unit,io,toPrint>(n,s,e,m);break;
        default:break;
      }
    }
    void fmt(Tag r,bool io,Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal,bool toPrint=true) override;
  };
}
