/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// flat navigation node... we will use it to navigate the tree navigation
// not implemented yet

#include "items.h"

namespace AM5 {

  //to use on the menu structure itself
  //THINK:this is the root of range => iterators and fields range
  template<typename O=Void>
  class SelfNav:public O {
    public:
      // using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
      using O::O;
      constexpr static inline bool canNav() {return true;}
      constexpr static inline bool sharedNav() {return false;}
      inline size_t pos() const {return sel;}
      inline void idx(size_t i) {sel=i;}
      inline bool down() {
        if (sel>0) {
          sel--;
          return true;
        }
        return false;
      }
      inline bool up() {
        if (sel<O::size()-1) {
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
  };

  //hold navigation state
  //flat navigation (single node)
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
      // inline Item& getFocus() {return getTarget();}

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

  // allow items to handle navigation (needed for fields)
  // items can handle up|down|enter|esc
  // left|right are a thing of the navigation system that can steal
  // focus from the field, an enter is sent to the field instead, to validate the entry
  template<typename O>
  class ItemNav:public O {
    public:
      using O::O;
      ItemNav() {assert(O::canNav());}
      constexpr inline bool canNav() {
        return focus||O::canNav();//we only have focus when it can nav
      }
      // inline Item& getFocus() {
      //   return focus?focus.getClient():O::getTarget();
      // }
      inline bool down() {
        return focus?focus.down():O::down();
      }
      inline bool up() {
        return focus?focus.up():O::up();
      }
      inline bool left() {
        if (focus) {
          focus.enter();
          focus=CmdAgent();
        }
        return O::getTarget().left();
      }
      inline bool right() {
        if (focus) {
          focus.enter();
          focus=CmdAgent();
        }
        return O::getTarget().right();
      }
      inline bool enter() {
        if (focus) {
          if (focus.enter()) return true;
          focus=Empty::activate();//blur if enter return false
        } else {
          focus=O::getTarget()[O::pos()].activate();
          if (focus.result()) return true;
        }
        return O::enter();
      }
      inline bool esc() {
        if (focus) {
          if (focus.esc()) focus=Empty::activate();
          return true;
        }
        return O::esc();
      }
    protected:
      NavAgent focus;
  };

  //provide all nav info for the composed chain but redirects calls to a common nav object
  //done so they can be non virtual and even static inlines
  //separating nav and output would either require more virtuals or a
  //very long chain of types and extra parameters, because items need also to access the
  //printers they would demand virtual as they can not be customized to outputs
  template<typename O,typename N,N& nav>
  class SharedNavNode:public O {
    public:
      // using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
      using O::O;
      constexpr static inline bool canNav() {return nav.canNav();}
      constexpr static inline bool sharedNav() {return true;}
      static inline size_t pos() {return nav.pos();}
      template<typename P>
      static inline bool selected(PrintHead<P> p) {return nav.selected(p);}
      static inline void setTarget(Item& i) {nav.setTarget(i);}
      static inline Item& getTarget() {return nav.getTarget();}
      // static inline Item& getFocus() {return nav.getFocus();}
      static inline void idx(size_t i) {nav.idx(i);}
      static inline bool down() {return nav.down();}
      static inline bool up() {return nav.up();}
      static inline bool left() {return nav.left();}
      static inline bool right() {return nav.right();}
      static inline bool enter() {return nav.enter();}
      static inline bool esc() {return nav.esc();}
  };


};//AM5 namespace
