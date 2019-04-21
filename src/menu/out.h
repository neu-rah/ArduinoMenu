/* -*- C++ -*- */
#pragma once

#include "base.h"
namespace AM5 {
  //////////////////////////////////////////////////////
  // output
  template<typename O=Nil>
  struct Void:public O {
    constexpr static inline bool isRange() {return false;}
    template<typename,typename,bool,size_t> static inline void fmtMenu() {}
    template<typename,typename,bool,size_t> static inline void fmtMenuBody() {}
    template<typename,typename,bool,size_t> static inline void fmtTitle() {}
    template<typename,typename,bool,size_t> static inline void fmtItem() {}
    template<typename,typename,bool,size_t> static inline void fmtIndex() {}
    template<typename,typename,bool,size_t> static inline void fmtCursor() {}
    template<typename T> static inline void raw(T) {}
    static inline void nl() {}
  };

  //static panel ------------------------------------------------
  // describes output geometry,
  // may be whole device, but must not exceed
  // it has origin coordinates to be displaced around
  template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
  struct StaticPanel:public O {
    constexpr static inline idx_t orgX() {return x;}
    constexpr static inline idx_t orgY() {return y;}
    constexpr static inline idx_t width() {return w;}
    constexpr static inline idx_t height() {return h;}
  };

  //its different than a scroll viewport
  //as it refers to the top line of the menu structure
  //minimize printing on line menus
  template<typename O>
  class RangePanel:public O {
    public:
      constexpr static inline bool isRange() {return true;}
      inline size_t top() const {return topLine;}
      inline void setTop(size_t n) {topLine=n;}
    protected:
      size_t topLine=0;
  };

  //track space usage
  template<typename O>
  class Viewport:public O {
    public:
      // using O::O;
      inline Viewport() {newView();}
      inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
      inline operator bool() const {return fx&&fy;}
      inline operator int() const {return free();}
      inline void newView() {
        fx=O::width();fy=O::height();}
      //TODO: need font size and char measure API
      inline void nl() {useY(1);}
      //device coordinates ---------
      inline idx_t posX() const {return (O::width()-fx)+O::orgX();}
      inline idx_t posY() const {return (O::height()-fy)+O::orgY();}
      // get free space ----
      inline idx_t freeX() const {return fx;}
      inline idx_t freeY() const {return fy;}
      inline size_t height() const {return freeY();}
      inline idx_t free() const {return fx+O::width()*fy;}
      // use space ----
      inline void useX(idx_t ux=1) {if (fx) fx-=ux; else useY();}
      inline void useY(idx_t uy=1) {
        if (!fy) {
          fx=0;
          fy=0;
        } else {
          fy-=uy;
          fx=O::width();
        }
      }
    protected:
      idx_t fx,fy;
  };

};
