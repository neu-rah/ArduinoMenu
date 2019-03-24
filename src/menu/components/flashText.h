/* -*- C++ -*- */
#pragma once

#include <menu.h>

namespace AM5 {
  template<typename O>
  class FlashTextDef:public O {
  protected:
    const char *text PROGMEM;
  public:
    FlashTextDef(PGM_P t):text(t) {}
    template<typename Out>
    inline size_t out(Out& o) {
      o.raw(reinterpret_cast<const __FlashStringHelper *>(text));
      return O::out(o);//chain the call
    }
  };
};//AM5
