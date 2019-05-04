/* -*- C++ -*- */
#pragma once

#include <menu.h>

namespace Menu {
  template<typename O>
  class EnDis:public O {
    public:
      inline bool enabled() const {return en;}
      inline void enable(bool b) {en=b;}
    protected:
      bool en=true;
  };
};
