/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// flat navigation node... we will use it to navigate the tree navigation
// not implemented yet

#include "items.h"
#include "arduino_assert.h"

namespace Menu {

  template<typename O=Void>
  class NavNode:public O {
    public:
      // using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
      using O::O;
      constexpr static inline bool canNav() {return true;}
      constexpr static inline bool sharedNav() {return false;}
      inline size_t pos() const {return sel;}
      template<typename P>
      inline bool selected(PrintHead<P> p) const {return sel==p.pos;}

      inline void setTarget(Item& i) {target=&i;}
      inline Item& getTarget() {return *target;}

      inline void idx(size_t i) {sel=i;}
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

  template<typename O,NavNode<Void>& nav>
  class SharedNavNode:public O {
    public:
      // using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
      using O::O;
      constexpr static inline bool canNav() {return true;}
      constexpr static inline bool sharedNav() {return true;}
      static inline size_t pos() {return nav.pos();}
      template<typename P>
      static inline bool selected(PrintHead<P> p) {return nav.selected(p);}
      static inline void setTarget(Item& i) {nav.setTarget(i);}
      static inline Item& getTarget() {return nav.getTarget();}
      static inline void idx(size_t i) {nav.idx(i);}
      static inline bool down() {return nav.down();}
      static inline bool up() {return nav.up();}
      static inline bool left() {return nav.left();}
      static inline bool right() {return nav.right();}
      static inline bool enter() {return nav.enter();}
      static inline bool esc() {return nav.esc();}
  };


};//Menu namespace
