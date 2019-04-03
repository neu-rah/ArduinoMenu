/* -*- C++ -*- */
#pragma once

#include "items.h"

namespace Menu {

  template<typename O>
  class NavNode:public O {
    public:
      using O::O;
      constexpr static inline bool canNav() {return true;}
      inline size_t pos() const {return sel;}
      inline bool selected(PrintHead p) const {return sel==p.pos;}
      inline void idx(size_t i) {sel=idx;}
      inline void up() {if (sel>0) sel--;}
      inline void down() {if (sel<size()-1) sel++;}
      static inline void left() {up();}
      static inline void right() {down();}
      static inline void enter() {}
      static inline void esc() {}
    protected:
      size_t sel=0;
  };

};//Menu namespace
