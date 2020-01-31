/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev>
struct SerialOut {
  template<typename O>
  struct Part:TextMeasure<1,1>::Part<O> {
    inline static void nl() {dev.println();}
    template<typename T> inline static void raw(T o) {dev.print(o);}
  };
};
