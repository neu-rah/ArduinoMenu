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
      inline void enable(bool b) {en=b;}
    protected:
      bool en=true;
  };

  //because static menu item have unique types
  template<typename I>
  class StaticEnDis:public I {
    public:
      using I::I;
      inline bool enabled() const {return en;}
      inline void enable(bool b) {
        _trace(MDO<<"enabling "<<b<<endl);
        en=b;}
    protected:
      static bool en;
  };

  #include "endis.hpp"
// };
