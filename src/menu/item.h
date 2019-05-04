/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu menu item base and some common items

#include "base.h"

namespace Menu {
  //structure item base--------------------------------------------------
  template<typename O=Nil>
  struct Empty {
    // template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    // static inline void printItem() {}
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItems(OutHead& out,ItemHead& item) {
      item.template printItem<NavHead,OutHead,ItemHead,idx>(out);
    }
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(ItemHead& item) {
      OutHead::template printMenu<NavHead,OutHead,ItemHead,0>(item);
    }
  };

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------

  struct Item {
    // virtual void out(MenuOut& o) const {}
    // virtual size_t size() const {return 1;}
    // virtual Item& operator[](size_t)=0;// const {return *this;}
    // virtual NavAgent activate()=0;// {assert(false);return CmdAgent();};
    virtual void printTo(MenuOutBase& out) {}
  };

  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    // inline void out(MenuOut& o) const override {O::out(o);}
    // size_t size() const override {return O::size();}
    // Item& operator[](size_t n) override {return O::operator[](n);}
    // inline NavAgent activate() override {return O::activate();}
    // //not used yet --
    // template<template<typename> class T>
    // inline void stack(MenuOut& o) const {Prompt<T<O>>(*this).out(o);}
    void printTo(MenuOutBase& out) override {
      O::template printItem<MenuNavBase,MenuOutBase,Item,0>(out);
    }
  };

  template<const char** text,typename O=Empty<>>
  struct StaticText:public O {
    using This=StaticText<text,O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItem(OutHead& out) {
      // cout<<"StaticText::printItem"<<endl;
      out.raw(text[0]);
    }
  };

  template<typename O,typename... OO>
  class StaticList:public StaticList<O> {
    public:
      using This=StaticList<O>;
      using Next=StaticList<OO...>;
      template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
      inline void printItems(OutHead& out,ItemHead& item) {
        // cout<<"StaticList...::printItems"<<endl;
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        out.template fmtItem<NavHead,OutHead,ItemPrinter,true,idx>(out);
        ItemPrinter::template printItem<NavHead,OutHead,This,idx>(out);
        out.template fmtItem<NavHead,OutHead,ItemPrinter,false,idx>(out);
        next.template printItems<NavHead,OutHead,Next,idx>(out,next);
      };
    protected:
      static Next next;
  };

  template<typename O,typename... OO>
  StaticList<OO...> StaticList<O,OO...>::next;

  template<typename O>
  struct StaticList<O>:public O {
    using This=StaticList<O>;
  };

};
