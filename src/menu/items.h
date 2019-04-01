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
    virtual void out(MenuOut& o) {}
    virtual size_t size() {return 1;}
    virtual Item& operator[](size_t) {return *this;}
  };

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    void out(MenuOut &o) override {
      O::out(o);
    }
    size_t size() override {return O::size();}
    Item& operator[](size_t n) override {return O::operator[](n);}
  };

  //static composition blocks -----------------------
  struct Empty {
    static inline void out(MenuOut&) {}
    static inline size_t size() {return 1;}
    inline Item& operator[](size_t n) {return *reinterpret_cast<Item*>(this);}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline void out(MenuOut &o) {o<<text;}
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
    inline Item& operator[](size_t i) const {return *data[i];}
  };

};//Menu
