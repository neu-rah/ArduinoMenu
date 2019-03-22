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

  // using Root=RootDef<Framework,Platform>;
  template<const char** text,typename O>
  struct StaticText:public O {
    using O::O;
    using Root=typename O::Root;
    // using Fmt=typename Root::Fmt;
    // template<Roles>
    template<typename Out>
    static inline void out(Out& o) {
      Serial<<"Text::out(Out)"<<endl;
      o<<text[0];
    }
  };

  template<typename O>
  class Text:public O {
  protected:
    const char *text;
  public:
    using Root=typename O::Root;
    // using Fmt=typename Root::Fmt;
    Text(const char* t):text(t) {}
    template<typename Out>
    inline void out(Out& o) const {o.raw(text);}
  };

  // template<typename O>
  // using Root=typename O::Root;

  template<typename Root,typename O>
  using asMenu=typename Root::template asMenu<O>;

  template<typename O>
  using Idx=typename O::Root::Idx;

  template<typename Q,Idx<Q> n>
  class StaticMenu:public asMenu<Q::Root,Q> {
    using O=asMenu<Q::Root,Q>;
    using Root=typename Q::Root;
    using Item=typename Root::Item;
    using Idx=typename Root::Idx;
  public:
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline Idx size() {return n;}
    // inline Item& operator[](Idx i) const {return *data[i];}
  protected:
    Item* data[n];
  };
};//AM5
