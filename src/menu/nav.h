/* -*- C++ -*- */
#pragma once

#include "items.h"

namespace Menu {

  template<typename O>
  class NavNode:public O {
    protected:
      size_t sel=0;
    public:
      using O::O;
      constexpr static inline bool canNav() {return true;}
      inline void up() {if (sel>0) sel--;}
      inline void down() {if (sel<size()-1) sel++;}
      static inline void left() {up();}
      static inline void right() {down();}
      static inline void enter() {}
      static inline void esc() {}
  };

};//Menu namespace
