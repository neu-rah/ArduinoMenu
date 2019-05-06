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
  struct Empty:public O {
    using O::O;
    // template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    // static inline void printItem() {}
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItems(NavHead& nav,OutHead& out,ItemHead& item) {
      item.template printItem<NavHead,OutHead,ItemHead,idx>(nav,out);
    }
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(ItemHead& item) {
      OutHead::template printMenu<NavHead,OutHead,ItemHead,0>(item);
    }
    template<size_t n>
    constexpr static inline bool enabled() {return true;}
    constexpr static inline bool enabled(size_t) {return true;}
    template<size_t n>
    static inline void enable() {}
    static inline void enable(size_t) {}
    // template<size_t n>
    // static inline void enable(bool) {}
  };

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------

  struct Item {
    // virtual void out(MenuOut& o) const {}
    // virtual size_t size() const {return 1;}
    // virtual Item& operator[](size_t)=0;// const {return *this;}
    // virtual NavAgent activate()=0;// {assert(false);return CmdAgent();};
    virtual void printTo(MenuNavBase& nav,MenuOutBase& out)=0;
    virtual inline bool enabled(size_t i) const=0;
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
    void printTo(MenuNavBase& nav,MenuOutBase& out) override {
      This::template printItem<MenuNavBase,MenuOutBase,Item,0>(nav,out);
    }
    inline bool enabled(size_t i) const override {return enabled(i);};
  };

  template<const char** text,typename O=Empty<>>
  struct StaticText:public O {
    using This=StaticText<text,O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItem(NavHead& nav,OutHead& out) {
      // cout<<"StaticText::printItem"<<endl;
      // using ItemPrinter=typename OutHead::Printers::template Item<This>;
      // out.template fmtItem<NavHead,OutHead,ItemPrinter,true,idx>(nav,out);
      // out.template fmtIndex<NavHead,OutHead,ItemPrinter,true,idx>(nav,out);
      // out.template fmtCursor<NavHead,OutHead,ItemPrinter,true,idx>(nav,out);
      out.raw(text[0]);
      // out.template fmtIndex<NavHead,OutHead,ItemPrinter,false,idx>(nav,out);
      // out.template fmtCursor<NavHead,OutHead,ItemPrinter,false,idx>(nav,out);
      // out.template fmtItem<NavHead,OutHead,ItemPrinter,false,idx>(nav,out);
    }
  };

  template<typename O,typename... OO>
  class StaticList:public StaticList<O> {
    public:
      using This=StaticList<O>;
      using Next=StaticList<OO...>;
      template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
      inline void printItems(NavHead& nav,OutHead& out,ItemHead& item) {
        // cout<<"StaticList...::printItems"<<endl;
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        out.template fmtItem<NavHead,OutHead,ItemPrinter,true,idx>(nav,out);
        ItemPrinter::template printItem<NavHead,OutHead,This,idx>(nav,out);
        out.template fmtItem<NavHead,OutHead,ItemPrinter,false,idx>(nav,out);
        next.template printItems<NavHead,OutHead,Next,idx>(nav,out,next);
      };
      // constexpr static inline size_t size() {return Next::size()+1;}
      template<size_t n>
      inline bool enabled() const {
        return n?next.template enabled<n-1>():O::enabled();
      }
      inline bool enabled(size_t n) const {return n?next.enabled(n):O::enabled(n);}
      template<size_t n>
      inline void enable(bool o) {
        return n?next.template enable<n-1>(o):O::enable(o);
      }
    protected:
      static Next next;
  };

  template<typename O,typename... OO>
  StaticList<OO...> StaticList<O,OO...>::next;

  template<typename O>
  struct StaticList<O>:public O {
    using This=StaticList<O>;
    template<size_t n>
    inline bool enabled() const {return n?true:O::enabled();}
    inline bool enabled(size_t n) const {return n?true:O::enabled(n);}
    template<size_t n>
    inline void enable(bool o) {if(!n) O::enable(o);}
  };

};
