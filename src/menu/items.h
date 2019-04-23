/* -*- C++ -*- */
#pragma once

namespace AM5 {
  ////////////////////////////////////////////////////////////
  // menu items

  //base for menu structure items
  template<typename O=Nil>
  struct Empty:public O {};

  //some static text
  template<const char**text,typename O=Empty<>>
  struct StaticText:public O {
    template<typename N>
    static inline void out() {
      N::raw(text[0]);
    }
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline Text(const Text<O>& t):text(t.text) {}
    inline void out(MenuOut &o) const {
      // o<<text;
      o.raw(text);
      O::out(o);
    }
  };

};//AM5 namespace
