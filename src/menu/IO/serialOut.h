/* -*- C++ -*- */
#pragma once
/**
* @file serialOut.h
* @author Rui Azevedo
* @brief Use arduino serial as menu output
*/

#include <menu.h>

// namespace Menu {
  template<typename Dev=decltype(Serial),Dev& dev=Serial,typename O=TextMeasure>
  struct SerialOut:public RawOut<Dev&,dev,O> {
    static inline void nl() {dev.println();}
    template<typename T> static inline void raw(T o) {dev.print(o);}
  };
// }
