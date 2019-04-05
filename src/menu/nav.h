/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// flat navigation node... we will use it to navigate the tree navigation
// not implemented yet

#include "items.h"

namespace Menu {

  template<typename O>
  class NavNode:public O {
    public:
      using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
      using O::O;
      constexpr static inline bool canNav() {return true;}
      inline size_t pos() const {return sel;}
      template<typename P>
      inline bool selected(PrintHead<P> p) const {return sel==p.pos;}

      inline void setTarget(Item& i) {target=&i;}
      inline Item& getTarget() {return *target;}

      inline void idx(size_t i) {sel=idx;}
      inline bool down() {
        if (sel>0) {
          sel--;
          return true;
        }
        return false;
      }
      inline bool up() {
        if (sel<(target?target->size()-1:0)) {
          sel++;
          return true;
        }
        return false;
      }
      static inline bool left() {return up();}
      static inline bool right() {return down();}
      static inline bool enter() {return false;}
      static inline bool esc() {return false;}
    protected:
      size_t sel=0;
      Item* target=NULL;
  };

};//Menu namespace
