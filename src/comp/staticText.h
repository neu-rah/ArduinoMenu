/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {
  template<const char** text,typename O=Empty>
  struct StaticTextDef:public O {
    template<typename H>
    static inline void out() {
      H::Printer::raw(text[0]);
    }
  };

};
