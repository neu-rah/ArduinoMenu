/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<decltype(Serial)& out,typename O=Void>
  struct SerialOutDef:public O {
    template<typename T>
    static inline void raw(T o) {out<<o;};
    static inline void nl() {out<<endl;}
  };
};
