/* -*- C++ -*- */
#pragma once

////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// panels describe output geometry


#include "../menu.h"
#include "out.h"

namespace Menu {

  //static panel
  //describes output dimension (may not be whole device)
  //but must not exceed
  //it has origin coordinates to be displaced around
  template<idx_t x,idx_t y,idx_t w,idx_t h,typename O=Void>
  struct StaticPanel:public O {
    constexpr static inline idx_t orgX() {return x;}
    constexpr static inline idx_t orgY() {return y;}
    constexpr static inline idx_t width() {return w;}
    constexpr static inline idx_t height() {return h;}
  };

  //dynamic panel, do we need this?
  // class Panel {
  //   public:
  //     inline idx_t posX() const {return x;}
  //     inline idx_t posY() const {return y;}
  //     inline idx_t width() const {return w;}
  //     inline idx_t height() const {return h;}
  //   protected:
  //   idx_t x,y,w,h;
  // };

  ////////////////////////////////////////////////////
  // views describe the progressin of a panel, tracking its free spece during print
  // its nor really a clipping, we are just accounting for variable size options

  //we dont care... as in serial device, we just print
  // struct FreeView {
  //   inline FreeView() {}
  //   static inline idx_t freeX() {return -1;}
  //   static inline idx_t freeY() {return -1;}
  //   static inline idx_t free() {return -1;}
  //   static inline idx_t posX() {return 0;}
  //   static inline idx_t posY() {return 0;}
  //   static inline void useX(idx_t ux=1) {}
  //   static inline void useY(idx_t uy=1) {}
  //   inline operator bool() const {return true;}
  //   inline operator int() const {return free();}
  //   inline void newView() {}
  // };

  //track space usage
  template<typename O>
  class Viewport:public O {
    public:
      // using O::O;
      inline Viewport() {newView();}
      inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
      inline operator bool() const {return fx&&fy;}
      inline operator int() const {return free();}
      inline void newView() {fx=O::width();fy=O::height();}
      //TODO: need font size and char measure API
      inline void endl() {useY(1);}
      //device coordinates ---------
      inline idx_t posX() const {return (O::width()-fx)+O::orgX();}
      inline idx_t posY() const {return (O::height()-fy)+O::orgY();}
      // get free space ----
      inline idx_t freeX() const {return fx;}
      inline idx_t freeY() const {return fy;}
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

  //track space usage and scroll position
  // right now its just crap
  // template<typename O>
  // class ScrollViewport:public O {
  //   public:
  //     // using O::O;
  //     inline ScrollViewport(idx_t x=0,idx_t y=0):sx(x),sy(y) {}
  //     inline ScrollViewport(const ScrollViewport<O>& o,idx_t x=0,idx_t y=0):O(o),sx(x),sy(y) {}
  //     inline operator bool() const {return freeY()&&freeX();}
  //     inline void vScrl(idx_t n) {
  //       // Serial<<"vScrl "<<n<<endl;
  //       sy-=n;}
  //     inline void hScrl(idx_t n) {sx-=n;}
  //     inline void vScrlTo(idx_t n) {
  //       // Serial<<"vScrlTo "<<n<<endl;
  //       sy=n;}
  //     inline void hScrlTo(idx_t n) {sx=n;}
  //     inline void scrl(idx_t x,idx_t y) {sx-=x;sy-=y;}
  //     inline void scrlTo(idx_t x,idx_t y) {sx=x;sy=y;}
  //     inline idx_t scrlPosX() const {return sx;}
  //     inline idx_t scrlPosY() const {return sy;}
  //     // inline idx_t width() const {return O::width()+sx;}
  //     // inline idx_t height() const {return O::height()+sy;}
  //     inline idx_t freeX() const {return O::freeX()-sx;}
  //     inline idx_t freeY() const {return O::freeY()-sy;}
  //     inline idx_t free() const {return width()*freeY()-(width()-freeX());}
  //     // inline idx_t posX() const {return O::posX()-sx;}
  //     // inline idx_t posY() const {return O::posY()-sy;}
  //   protected:
  //     idx_t sx,sy;//scroll positions
  // };

};//Menu
