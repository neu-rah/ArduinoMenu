/* -*- C++ -*- */
#pragma once

#include "../base.h"

namespace AM5 {
  template<typename O>
  class EnDis:public O {
    public:
      inline bool enabled() const {return en;}
      inline void enable(bool b) {en=b;}
    protected:
      bool en=true;
  };
};
