/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev>
struct SerialOut {
  template<typename O>
  struct As:TextMeasure<1,1>::As<O> {
    inline static void nl() {dev.println();}
    template<typename T> inline static void raw(T o) {dev.print(o);}
  };
};
