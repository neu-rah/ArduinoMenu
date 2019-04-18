/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<typename T,T text,typename O=Empty>
  struct StaticFlashTextDef:public O {
    template<typename H>
    static inline void out() {
      H::Printer::raw(
        reinterpret_cast<const __FlashStringHelper *>(
          text[0]
        )
      );
    }
  };
};
