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
      template<typename P>
      inline bool selected(PrintHead<P> p) const {return sel==p.pos;}
      inline void idx(size_t i) {sel=idx;}
      inline void up() {if (sel>0) sel--;}
      inline void down() {if (sel<(target?target->size()-1:0)) sel++;}
      static inline void left() {up();}
      static inline void right() {down();}
      static inline void enter() {}
      static inline void esc() {}
    protected:
      size_t sel=0;
      Item* target=NULL;
  };

};//Menu namespace
