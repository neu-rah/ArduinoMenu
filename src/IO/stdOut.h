/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<ostream& out,typename O=Void>
  struct StdOutDef:public O {
    template<typename T>
    static inline void raw(T o) {out<<o;};
    static inline void nl() {out<<endl;}
  };
};
