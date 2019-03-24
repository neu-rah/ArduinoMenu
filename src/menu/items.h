/* -*- C++ -*- */
#pragma once

#include <menu.h>

namespace AM5 {
  /////////////////////////////////////////////////////////
  //static routers
  // struct Empty {
  //   using OutDef=Out;
  //   Empty() {}
  //   static inline RawOut& out(RawOut& o) {return o;}
  //   static inline Idx size() {return 0;}
  //   inline Base& operator[](Idx n) const {
  //     _trace(MENU_DEBUG_OUT<<"Item not available as non-constant"<<endl);
  //     while(true);
  //   }
  // };

  template<typename T,typename O>
  class DataDef:public O {
  protected:
    T data;
  public:
    using O::O;
    inline DataDef(T t):data(t) {}
    template<typename Out>
    inline size_t out(Out& o) const {return o.raw(data);}
  };

  // using Root=RootDef<Framework,Platform>;
  template<const char** text,typename O>
  struct StaticTextDef:public O {
    using O::O;
    inline StaticTextDef() {}
    template<typename Out>
    static inline size_t out(Out& o) {return o.raw(text[0]);}
  };

  template<typename O>
  class TextDef:public O {
  protected:
    const char *text;
  public:
    using Root=typename O::Root;
    TextDef(const char* t):text(t) {}
    template<typename Out>
    inline size_t out(Out& o) const {return o.raw(text);}
  };

  template<typename O>
  using asMenu=typename O::Root::template asMenu<O>;

  template<typename O>
  using Idx=typename O::Root::Idx;

  template<typename Q,Idx<Q> n>
  class StaticMenuDef:public asMenu<Q> {
    using O=asMenu<Q>;
    using Root=typename Q::Root;
    using Item=typename Root::Item;
    using Idx=typename Root::Idx;
  public:
    template<typename... OO>
    inline StaticMenuDef(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenuDef(const char*title,OO... oo):O(title),data{oo...} {}
    static inline Idx size() {return n;}
    // inline Item& operator[](Idx i) const {return *data[i];}
  protected:
    Item* data[n];
  };
};//AM5
