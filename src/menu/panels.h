/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu output space management
*/

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

  constexpr static inline idx_t posX() {return x;}
  constexpr static inline idx_t posY() {return y;}
  constexpr static inline idx_t freeX() {return w;}
  constexpr static inline idx_t freeY() {return h;}
  constexpr static inline idx_t free() {return w*h;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}
};

// template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
// struct StaticPanel:public _StaticPanel<x,y,w,h,O> {};
//
// template<idx_t w,idx_t h,typename O>
// struct StaticPanel<0,0,w,h,O>:public _StaticPanel<0,0,w,h,O> {};

//its different than a scroll viewport
//as it refers to the top line of the menu structure
//minimize printing on line menus
template<typename O>
class RangePanel:public O {
  public:
    constexpr static inline bool isRange() {return true;}
    inline idx_t top() const {return topLine;}
    inline void setTop(idx_t n) {topLine=n;}
    // inline idx_t posY() const {return O::posY()-top();}
  protected:
    idx_t topLine=0;
};

//track space usage
template<typename O>
class Viewport:public O {
  public:
    // using O::O;
    using This=Viewport<O>;
    inline Viewport() {/*newView();*/}
    inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
    constexpr static inline bool isViewport() {return true;}
    inline operator bool() const {return fx&&fy;}
    inline operator int() const {return free();}
    inline void newView() {
      _trace(MDO<<"newView()"<<endl);
      fx=O::width();fy=O::height();//+O::top();
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
