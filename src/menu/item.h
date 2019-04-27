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
    // static inline void printTo() {}
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItems(ItemHead& item) {
      item.template printTo<NavHead,OutHead,ItemHead,idx>();
    }
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(ItemHead& item) {
      OutHead::template printMenu<NavHead,OutHead,ItemHead,0>(item);
    }
  };

  //--------------------------------------------------
  struct Item {
    //footprint:
    // 4 bytes for each virtual function * #virtual tables
    // the # of vtables is equal to the # of unique Prompt<...> compositions
    // virtual void out(MenuOut& o) const {}
    // virtual size_t size() const {return 1;}
    // virtual Item& operator[](size_t)=0;// const {return *this;}
    // virtual NavAgent activate()=0;// {assert(false);return CmdAgent();};
    virtual void printTo() {cout<<"!";}
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
  };

  template<const char** text,typename O=Empty<>>
  struct StaticText:public O {
    using This=StaticText<text,O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printTo() {
      // cout<<"StaticText::printTo"<<endl;
      OutHead::raw(text[0]);
    }
  };

  template<typename O,typename... OO>
  class StaticList:public StaticList<O> {
    public:
      using This=StaticList<O>;
      using Next=StaticList<OO...>;
      template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
      static inline void printItems(ItemHead& item) {
        // cout<<"StaticList...::printItems"<<endl;
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        OutHead::template fmtItem<NavHead,OutHead,ItemPrinter,true,idx>();
        ItemPrinter::template printTo<NavHead,OutHead,This,idx>();
        OutHead::template fmtItem<NavHead,OutHead,ItemPrinter,false,idx>();
        next.template printItems<NavHead,OutHead,Next,idx>(next);
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
