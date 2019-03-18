/* -*- C++ -*- */
#pragma once

#include <base/roles.h>
#include <base/debug.h>

template<typename Out>
struct MenuSystemDef {
  //////////////////////////////////////////////////
  // interface
  struct Base {
    inline virtual Out& operator<<(Out& o) const {return o;}
    inline virtual size_t size() const {return 0;}
    inline virtual Base& operator[](size_t n) const {
      _trace(MENU_DEBUG_OUT<<"Item not available as non-constant"<<endl);
      throw 1;
    }
  };

  //adapter
  template<typename Q>
  struct Item:public Base,public asItem<Q> {
    using O=asItem<Q>;
    using O::O;

    template<typename... OO>
    inline Item(OO... oo):O(oo...) {}
    template<typename... OO>
    inline Item(const char*title,OO... oo):O(title,oo...) {}
    inline Item(const char*title):O(title) {}

    inline Out& operator<<(Out& o) const override {return O::out(o);}
    inline size_t size() const override {return O::size();}
    inline Base& operator[](size_t n) const override {return O::operator[](n);}
  };

  /////////////////////////////////////////////////////////
  //static routers
  struct Empty {
    static inline Out& out(Out& o) {return o;}
    static inline size_t size() {return 0;}
    inline Base& operator[](size_t n) const {
      _trace(MENU_DEBUG_OUT<<"Item not available as non-constant"<<endl);
      throw 1;
    }
  };

  template<const char** text,typename O=Empty>
  struct StaticText:public O {
    static inline Out& out(Out& o) {return o<<text[0];}
  };

  template<typename O=Empty>
  class Text:public O {
  protected:
    const char *text;
  public:
    Text(const char* t):text(t) {}
    inline Out& out(Out& o) const {return o<<text;}
  };

  template<size_t n,typename Q=Empty>
  class StaticMenu:public asMenu<Q> {
  protected:
    Base* data[n];
  public:
    using O=asMenu<Q>;
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline Base& operator[](size_t i) const {return *data[i];}
  };
};

template<typename Out,typename O>
inline Out& operator<<(Out& out,O& item) {
  return item.operator<<(out);
}
