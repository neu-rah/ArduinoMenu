/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  template<typename Out,typename Target>
  struct NavDef {
    //base for navigation system :)
    class Drift {
      public:
        //navigation API ------------------------
        constexpr static inline size_t pos() {return 0;}
        constexpr static inline bool up() {return false;}
        constexpr static inline bool down() {return false;}
        constexpr static inline bool left() {return down();}
        constexpr static inline bool right() {return up();}
        constexpr static inline bool enter() {return false;}
        constexpr static inline bool esc() {return false;}
        // Output API redirection --------------------------------
        template<typename N,typename T,N& n>
        inline void raw(T o) {menuOut.template raw<N,T,n>(o);};
        template<typename N,N& n>
        inline void nl() {menuOut.template nl<N,n>();};
        template<size_t i>
        inline void enable(bool b=true) {target.template enable<i>(b);}
        template<size_t i>
        inline bool enabled() const {return target.template enabled<i>();}
        inline bool isRange() const {return menuOut.isRange();}
        inline size_t top() const {return menuOut.top();}
        inline void setTop(size_t o) {menuOut.setTop(o);}
        inline idx_t height() const {return menuOut.height();}
        //output panels and viewports (part of output API)
        inline idx_t posX() const {return menuOut.posX();}
        inline idx_t posY() const {return menuOut.posY();}
        inline void useX(idx_t ux=1) const {menuOut.useX(ux);}
        inline void useY(idx_t uy=1) {menuOut.useY(uy);}
        // menu structure API redirection
        inline size_t size() const {return target.size();}
        template<typename H,size_t n>
        inline void printItems(H& ph) {target.template printItems<H,n>(ph);}
        //aux functions
        inline Target& getTarget() {return target;}
        inline Out& getOut() {return menuOut;}
      protected:
        Out menuOut;
        Target target;
    };

    template<typename O=Drift>
    class PosDef:public O {
      public:
        inline bool selected(size_t n) const {return at==n;}
        inline bool up() {
          if (at<O::size()-1) {at++;return true;}
          return O::up();
        }
        inline bool down() {
          if (at>0) {at--;return true;}
          return O::down();
        }
        inline size_t pos() const {return at;}
      protected:
        size_t at=0;
    };
    //enclose navigation system and provides higher level functions
    //to hide some abstrations
    template<typename O>
    struct Root:public O {
      using This=Root<O>;
      using NavOut=Out;
      using NavTarget=Target;
      inline void printMenu() {O::menuOut.template printMenu<This>(*this);}
    };
  };
};
