/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<
    typename Dev=decltype(Serial),
    Dev& device=Serial,
    typename O=Void<>
  > struct SerialOut:public O {
    using This=SerialOut<Dev,device,O>;
    template<typename T>
    static inline void raw(T o) {device.print(o);}
    static inline void nl() {device.println();}
    static inline This& endl(This& o) {nl();return o;}
  };
};//AM5 namespace
