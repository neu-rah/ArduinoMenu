/* -*- C++ -*- */
#pragma once

#include "../menu.h"
#include "out.h"

namespace Menu {

  // template<typename I>
  // struct ItemIterator {
  //   size_t at=0;
  //   virtual inline I operator*() {return data[at];}
  //   virtual inline I operator==(const It o) const {return at==o.at;}
  //   virtual inline I operator!=(const It o) const {return !operator==(o);}
  //   virtual inline I operator++() {return data[++at];}
  //   virtual inline I operator++(I) {return data[at++];}
  // };

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------
  struct Item {
    virtual void out(MenuOut& o) const {}
    #if (MENU_INJECT_PARTS==true)
      virtual void out(MenuOut& o,PrinterPart& pp) const {}
    #endif
    virtual size_t size() const {return 1;}
    virtual const Item& operator[](size_t) const {return *this;}
    virtual bool canNav() const {return false;}
    virtual bool up() {return false;}
    virtual bool down() {return false;}
    virtual bool enter() {return false;}
    virtual bool esc() {return false;}
  };

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    inline void out(MenuOut& o) const override {O::out(o);}
    #if (MENU_INJECT_PARTS==true)
      void out(MenuOut& o,PrinterPart& pp) const override;
    #endif
    size_t size() const override {return O::size();}
    const Item& operator[](size_t n) const override {return O::operator[](n);}
    bool canNav() const override {return O::canNav();}
    bool up() override {return O::up();}
    bool down() override {return O::down();}
    bool enter() override {return O::enter();}
    bool esc() override {return O::esc();}
    //type injection, alls should have copy constructor and they must align
    template<template<typename> class T>
    inline void stack(MenuOut& o) const {Prompt<T<O>>(*this).out(o);}
  };

  #if (MENU_INJECT_PARTS==true)
    struct PrinterPart {
      template<typename O>
      void use(MenuOut& o,Prompt<O>& i) {
        Serial<<"PrinterPart::use..."<<endl;
        Prompt<O>(i).out(o);
        // i.stack<part>(o);
      }
    };
  #endif

  #if (MENU_INJECT_PARTS==true)
    template<typename O>
    void Prompt<O>::out(MenuOut& o,PrinterPart& pp) const {
      pp.use<O>(o,*this);
    }
  #endif

  //static composition blocks -----------------------
  struct Empty {
    inline Empty() {}
    inline Empty(Empty&) {}
    static inline void out(MenuOut&) {}
    static inline size_t size() {return 1;}
    inline const Item& operator[](size_t n) const {return *reinterpret_cast<const Item*>(this);}
    constexpr static inline bool canNav() {return false;}
    static inline bool up() {return false;}
    static inline bool down() {return false;}
    static inline bool enter() {return false;}
    static inline bool esc() {return false;}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline Text(const Text<O>& t):text(t.text) {}
    inline void out(MenuOut &o) const {
      o<<text;
      O::out(o);
    }
  };

  template<size_t n,typename O>
  class StaticMenu:public O {
  protected:
    Item* data[n];
  public:
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline const Item& operator[](size_t i) const {return *data[i];}
  };

};//Menu
