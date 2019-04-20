/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<typename T,T* text,typename O=Empty<>>
  struct FlashText:public O {
    template<typename H>
    static inline void out(H& ph) {
      ph.menuOut.template raw<
        typename H::NavRoot,
        const __FlashStringHelper *,
        ph.nav
      >(reinterpret_cast<const __FlashStringHelper *>(text[0]));
    }
  };
};
