/* -*- C++ -*- */
#pragma once

#include "../base.h"
#include <LiquidCrystal.h>

namespace AM5 {
  template<LiquidCrystal& device,typename O=Void<>>
  struct LCDOut:public O {
    using This=LCDOut<device,O>;
    template<typename T>
    static inline void raw(T o) {device.print(o);}
    static inline void nl() {
      //device<<std::endl;
    }
    static inline StdOut& endl(StdOut& o) {nl();return o;}
  };
};//AM5 namespace
