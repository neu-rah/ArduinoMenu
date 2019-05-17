/* -*- C++ -*- */
#pragma once

#include <menu.h>

// namespace Menu {
  template<typename O>
  class EnDis:public O {
    public:
      inline bool enabled(idx_t i) const {return i?true:en;}
      inline void enable(idx_t,bool b) {
        _trace(MDO<<(b?"enable":"disable")<<endl);
        en=b;
      }
    protected:
      bool en=true;
  };
// };
