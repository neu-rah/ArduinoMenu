/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu output base

#include "base.h"

namespace Menu {
  //output base-------------------------------------------------------
  template<typename O=Nil>
  struct Void {
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw() {}
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtItem() {}
  };
};
