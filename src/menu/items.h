/* -*- C++ -*- */
#pragma once

namespace AM5 {
  ////////////////////////////////////////////////////////////
  // menu items
  template<typename O=Nil>
  struct Empty:public O {};

  template<const char**text,typename O=Empty<>>
  struct StaticText:public O {
    template<typename N>
    static inline void out() {
      N::raw(text[0]);
    }
  };
};//AM5 namespace
