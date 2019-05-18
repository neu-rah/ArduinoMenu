/* -*- C++ -*- */
#pragma once

#include <menu.h>

// namespace Menu {
  template<typename O>
  class EnDis:public O {
    public:
      inline bool enabled() const {return en;}
      inline bool enabled(idx_t i) const {return i?O::enabled(i):en;}
      inline void enable(idx_t,bool b) {
        trace(MDO<<(b?"enable":"disable")<<endl);
        en=b;
      }
    protected:
      bool en=true;
  };
// };
