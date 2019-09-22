/* -*- C++ -*- */
#pragma once

#include "api.h"

//Output Device Operation
enum class OutOps {RawOut,Measure};
template<OutOps> struct OutOp {};

using RawOutOp=OutOp<OutOps::RawOut>;
using MeasureOp=OutOp<OutOps::Measure>;

////////////////////////////////////////////////////////////////////////////////
// panels and viewports
template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
struct StaticPanel:public O {
  constexpr static inline idx_t orgX() {return x;}
  constexpr static inline idx_t orgY() {return y;}
  constexpr static inline idx_t width() {return w;}
  constexpr static inline idx_t height() {return h;}

  constexpr static inline idx_t posX() {return x;}
  constexpr static inline idx_t posY() {return y;}
  constexpr static inline idx_t freeX() {return w;}
  constexpr static inline idx_t freeY() {return h;}
  constexpr static inline idx_t free() {return w*h;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}
};

template<typename O>
class RangePanel:public O {
  public:
    using This=RangePanel<O>;
    constexpr static inline bool isRange() {return true;}
    // inline bool inRange(idx_t n) {return n-topLine<O::height();}
    inline idx_t top() const {return topLine;}
    inline void setTop(idx_t n) {topLine=n;}
    inline void newView() {
      _trace(MDO<<"RangePanel::newView()"<<endl);
      freeLines=O::height();
    }
    inline void useY(idx_t uy=1) {freeLines-=uy;}
    inline void nl() {O::nl();This::useY();}
    inline idx_t freeY() const {return freeLines;}
  protected:
    idx_t topLine=0;
    idx_t freeLines;
};

template<typename O>
class Viewport:public O {
  public:
    // using O::O;
    using This=Viewport<O>;
    inline Viewport() {}
    inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
    constexpr static inline bool isViewport() {return true;}
    inline operator bool() const {return fx&&fy;}
    inline operator int() const {return free();}
    inline void newView() {
      trace(MDO<<"newView()"<<endl);
      fx=O::width();fy=O::height();
      //O::newView();
    }
    inline void nl() {O::nl();This::useY();}
    template<typename T>
    inline void raw(T i) {
      O::setCursor(This::posX(),This::posY());
      O::raw(i);
      This::useX(O::measure(i));
    }

    //device coordinates ---------
    inline idx_t posX() const {return (O::width()-fx)+O::orgX();}
    inline idx_t posY() const {return (O::height()-fy)+O::orgY();}
    // get free space ----
    inline idx_t freeX() const {return fx;}
    inline idx_t freeY() const {
      // Serial<<"Viewport::freeY "<<fy<<endl;
      return fy;}
    inline idx_t height() const {
      return freeY();}
    inline idx_t free() const {return fx+O::width()*fy;}
    // use space ----
    inline void useX(idx_t ux=1) {if (fx) fx-=ux; else useY();}
    inline void useY(idx_t uy=1) {
      if (!fy) {
        fx=0;
        // fy=0;
      } else {
        fy-=uy;
        fx=O::width();
      }
    }
  protected:
    inline void _nl(RawOutOp) {O::nl();}
    inline void _nl(MeasureOp) {O::useY();}
    template<typename T>
    inline void _raw(T i,RawOutOp) {O::raw(i);}
    template<typename T>
    inline void _raw(T i,MeasureOp) {O::measure(i);}
    idx_t fx,fy;
};
