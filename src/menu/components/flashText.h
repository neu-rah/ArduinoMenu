/* -*- C++ -*- */
#pragma once

#include <menu.h>

namespace AM5 {
  template<typename O>
  class FlashTextDef:public O {
  protected:
    // const char *text PROGMEM;
    __FlashStringHelper *text;
  public:
    FlashTextDef(PGM_P t)
      :text(reinterpret_cast<const __FlashStringHelper *>(t)) {}
    template<typename Out>
    inline size_t out(Out& o) {
      o.raw(text);
      return O::out(o);//chain the call
    }
  };
};//AM5
