/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// flat navigation node... we will use it to navigate the tree navigation
// not implemented yet

#include "items.h"

namespace AM5 {

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
        #if NAV_AGENT
          return focus?focus.down():O::down();
        #else
          return focus?focus->down():O::down();
        #endif
      }
      inline bool up() {
        #if NAV_AGENT
          return focus?focus.up():O::up();
        #else
          return focus?focus->up():O::up();
        #endif
      }
      inline bool left() {
        if (focus) {
          #if NAV_AGENT
            focus.enter();
            focus=CmdAgent();
          #else
            focus->enter();
            focus=NULL;
          #endif
        }
        return O::getTarget().left();
      }
      inline bool right() {
        if (focus) {
          #if NAV_AGENT
            focus.enter();
            focus=CmdAgent();
          #else
            focus->enter();
            focus=NULL;
          #endif
        }
        return O::getTarget().right();
      }
      inline bool enter() {
        Serial<<"enter"<<endl;
        if (focus) {
          Serial<<"focused field... passing"<<endl;
          #if NAV_AGENT
            if (focus.enter()) return true;
            Serial<<"quiting focus"<<endl;
            focus=CmdAgent();//blur if enter return false
          #else
            if (focus->enter()) return true;
            focus=NULL;
          #endif
        } else {
          Serial<<"no focus yet, checking item..."<<endl;
          #if NAV_AGENT
            focus=O::getTarget()[O::pos()].navAgent();
          #else
            if (O::getTarget()[O::pos()].navAgent())
              focus=&O::getTarget()[O::pos()];
          #endif
          Serial<<"focus is now:"<<O::getTarget()[O::pos()].navAgent().canNav()<<endl;
        }
        return O::enter();
      }
      inline bool esc() {
        if (focus) {
          #if NAV_AGENT
            if (focus.esc()) focus=CmdAgent();
          #else
            if (focus->esc()) focus=NULL;
          #endif
          return true;
        }
        return O::esc();
      }
    protected:
      #if NAV_AGENT
        CmdAgent focus;
      #else
        Item* focus=NULL;
      #endif
  };

  //provide all nav info for the composed chain but redirects calls to a common nav object
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
