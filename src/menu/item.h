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
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printTo() {
      // ItemHead::template out<NavHead,OutHead,ItemHead>();
    }
  };

  template<const char** text,typename O=Empty<>>
  struct StaticText:public O {
    using This=StaticText<text,O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printTo() {OutHead::raw(text[0]);}
  };
};
