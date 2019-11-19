/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev,typename O=TextMeasure>
struct SerialOut:O {
  inline static void nl() {dev.println();}
  template<typename T>
  inline static void raw(T o) {dev.print(o);}
};
