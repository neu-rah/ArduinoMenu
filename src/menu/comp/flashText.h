/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<typename T,T* text,typename O=Empty<>>
  struct FlashText:public O {
    template<typename N>
    static inline void out() {
      N::raw(reinterpret_cast<const __FlashStringHelper *>(text[0]));
    }
  };
};
