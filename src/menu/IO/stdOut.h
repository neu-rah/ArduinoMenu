/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<ostream& device=std::cout,typename O=Void<>>
  struct StdOut:public O {
    using This=StdOut<device,O>;
    template<typename Nav,typename T>
    static inline void raw(T o) {device<<o;}
    static inline void nl() {device<<std::endl;}
  };
};//AM5 namespace
