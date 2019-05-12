/* -*- C++ -*- */
#pragma once

#include <menu.h>

// namespace Menu {
  template<typename O>
  class EnDis:public O {
    public:
      inline bool enabled(size_t i) const {return i?true:en;}
      inline void enable(bool b) {en=b;}
    protected:
      bool en=true;
  };
// };
