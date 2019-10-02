/* -*- C++ -*- */
#pragma once
/**
* @file endis.h
* @author Rui Azevedo
* @brief menu item component, provides Enable/Disable functionality
*/

#include <menu.h>

// namespace Menu {
  /**
  * The EnDis class provides Enable/Disable functionality for menu items
  */
  template<typename I>
  class EnDis:public I {
    public:
      using I::I;
      inline bool enabled() const {return en;}
      inline bool enabled(idx_t i) const {return i?I::enabled(i):en;}
      inline void enable(idx_t,bool b) {
        trace(MDO<<(b?"enable":"disable")<<endl);
        en=b;
      }
    protected:
      bool en=true;
  };
// };
