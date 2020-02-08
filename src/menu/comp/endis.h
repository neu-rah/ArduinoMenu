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
  template<bool e=true>
  class EnDis {
    public:
      template<typename I>
      struct Part:public I {
        public:
          using I::I;
          inline bool enabled(PathRef=self) const {return isEnabled;}
          inline void enable(bool b,PathRef=self) {isEnabled=b;}
        protected:
          bool isEnabled=e;
      };
  };

  //because static menu item have unique types
  template<typename I>
  class StaticEnDis:public I {
    public:
      using I::I;
      inline bool enabled() const {return en;}
      inline void enable(bool b) {en=b;}
    protected:
      static bool en;
  };

  #include "endis.hpp"
// };
