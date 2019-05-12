/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu standard C++ stream output

#include <menu.h>

// namespace Menu {
  template<typename Dev=decltype(Serial),Dev& dev=Serial,typename O=FullPrinter<>>
  struct SerialOut:public RawOut<Dev&,dev,O> {
    static inline void nl() {dev.println();}
    template<typename T> static inline void raw(T o) {dev.print(o);}
  };
// }
