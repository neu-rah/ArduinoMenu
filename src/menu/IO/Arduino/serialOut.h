/* -*- C++ -*- */
#pragma once

#include "../../../staticMenu.h"

namespace Menu {
  template<typename Dev,Dev& dev>
  struct SerialOut {
    template<typename O>
    struct Part:TextMeasure<1,1>::Part<O> {
      template<bool toPrint=true>
      inline static void nl() {dev.println();}
      template<typename T,bool toPrint=true> inline static void raw(T o) {
        if(toPrint) dev.print(o);
      }
    };
  };
};
