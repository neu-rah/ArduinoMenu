/* -*- C++ -*- */
#pragma once

#include <base/roles.h>
#include <base/debug.h>

template<typename O,typename Raw=RawOutType>
struct OutDef:public O {
  using This=OutDef<Raw,O>;
  using RawOut=Raw;
};

template<typename Out>
struct MenuSystemDef {
  using SysDef=MenuSystemDef<Out>;
  using This=SysDef;
  using OutDef=typename Out::This;
  using RawOut=typename OutDef::RawOut;
  //////////////////////////////////////////////////
  // interface
  // keep vtable small!
  struct Base {
    inline virtual RawOut& operator<<(RawOut& o) const {return o;}
    inline virtual size_t size() const {return 0;}
    inline virtual Base& operator[](size_t n) const =0;
  };

  //adapter
  //a version of this (vtable only) is created for each unique combination type
  //should we multiply it by the number of output devices?
  //we already doing it for raw out, so, no big deal
  template<typename Q>
  struct Item:public Base,public asItem<Q> {
    using O=asItem<Q>;
    using O::O;

    template<typename... OO>
    inline Item(OO... oo):O(oo...) {}
    template<typename... OO>
    inline Item(const char*title,OO... oo):O(title,oo...) {}
    inline Item(const char*title):O(title) {}
    inline RawOut& operator<<(RawOut& o) const override {return O::out(o);}
    inline size_t size() const override {return O::size();}
    inline Base& operator[](size_t n) const override {return O::operator[](n);}
  };

  /////////////////////////////////////////////////////////
  //static routers
  struct Empty {
    using OutDef=Out;
    Empty() {}
    static inline RawOut& out(RawOut& o) {return o;}
    static inline size_t size() {return 0;}
    inline Base& operator[](size_t n) const {
      _trace(MENU_DEBUG_OUT<<"Item not available as non-constant"<<endl);
      while(true);
    }
  };

  template<const char** text,typename O=Empty>
  struct StaticText:public O {
    using O::O;
    // template<Roles>
    static inline RawOut& out(RawOut& o) {
      o<<text[0];
      return o;
    }
  };

  template<typename O=Empty>
  class Text:public O {
  protected:
    const char *text;
  public:
    Text(const char* t):text(t) {}
    // template<Roles>
    inline RawOut& out(RawOut& o) const {return o<<text;}
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

  //runtime compositions
  // template<typename O>
  // class Prefix:public O {
  // public:
  //   Prefix(Base& o):inner(o) {}
  //   template<Roles>
  //   inline RawOut& out(RawOut& o) const {
  //     O::out(o);
  //     return o<<inner;
  //   }
  // protected:
  //   Base& inner;
  // };
};

// template<typename Out,typename O>
// inline RawOut& operator<<(RawOut& out,O& item) {
//   item.operator<<(out);
//   return o;
// }
