/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
#include "../items.h"

namespace Menu {
  template<typename O=Empty>
  class FlashTextDef:public O {
  protected:
    const __FlashStringHelper * text;
  public:
    FlashTextDef(PGM_P t):text(reinterpret_cast<const __FlashStringHelper *>(t)) {}
    inline void out(MenuOut &o) {
      o<<text;
    }
  };
};
