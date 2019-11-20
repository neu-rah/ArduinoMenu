/* -*- C++ -*- */
#pragma once

/**************
This os part of the output system
*/

#include "api.h"

//Output Device Operation
enum class OutOps {RawOut,Measure};
template<OutOps> struct OutOp {};

using RawOutOp=OutOp<OutOps::RawOut>;
using MeasureOp=OutOp<OutOps::Measure>;

////////////////////////////////////////////////////////////////////////////////
// panels and viewports
template<Idx x,Idx y,Idx w,Idx h,typename O>
struct StaticPanel:public O {
  constexpr static inline Idx orgX() {return x;}
  constexpr static inline Idx orgY() {return y;}
  constexpr static inline Idx width() {return w;}
  constexpr static inline Idx height() {return h;}

  constexpr static inline Idx posX() {return x;}
  constexpr static inline Idx posY() {return y;}
  constexpr static inline Idx freeX() {return w;}
  constexpr static inline Idx freeY() {return h;}
  constexpr static inline Idx free() {return w*h;}
  static inline void useX(Idx ux=1) {}
  static inline void useY(Idx uy=1) {}
};

template<typename O,int w=1,int h=1>
class RangePanel:public O {
  public:
    using This=RangePanel<O>;
    inline Idx top() const {return topLine;}
    inline void setTop(Idx n) {topLine=n;}
    inline void newView() {
      trace(MDO<<"RangePanel::newView"<<endl);
      freeLines=O::height();
      O::setCursor(0,O::ascent());
    }
    inline void useY(Idx uy=h) {
      if (freeLines) {
        freeLines-=uy;
        trace(MDO<<"useY h:"<<O::height()<<"-freeLines:"<<freeLines<<"="<<O::height()-freeLines<<endl);
        O::setCursor(0,(O::height()-freeLines)*O::maxCharHeight()+O::ascent());
      }
    }
    inline void nl() {
      O::nl();
      useY();
    }
    inline Idx freeY() const {return freeLines;}
    template<typename Nav>
    inline void posTop(Nav& nav) {
      trace(MDO<<"RangePanel::posTop for "<<nav.pos()<<endl);
      while(top()>nav.pos()) setTop(top()-1);
      //TODO: this is NOT correct for multiline options!!!!
      while(nav.pos()>=top()+freeY()) setTop(top()+1);
      trace(MDO<<"top:"<<top()<<endl);
    }
  protected:
    Idx topLine=0;
    Idx freeLines;
};

// template<typename O>
// class Viewport:public O {
//   public:
//     // using O::O;
//     using This=Viewport<O>;
//     inline Viewport() {}
//     inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
//     constexpr static inline bool isViewport() {return true;}
//     inline operator bool() const {return fx&&fy;}
//     inline operator int() const {return free();}
//     inline void newView() {
//       trace(MDO<<"Viewport::newView"<<endl);
//       fx=O::width();fy=O::height();
//       //O::newView();
//     }
//     inline void nl() {O::nl();This::useY();}
//     template<typename T>
//     inline void raw(T i) {
//       O::setCursor(This::posX(),This::posY());
//       O::raw(i);
//       This::useX(O::measure(i));
//     }
//
//     //device coordinates ---------
//     inline Idx posX() const {return (O::width()-fx)+O::orgX();}
//     inline Idx posY() const {return (O::height()-fy)+O::orgY();}
//     // get free space ----
//     inline Idx freeX() const {return fx;}
//     inline Idx freeY() const {
//       // Serial<<"Viewport::freeY "<<fy<<endl;
//       return fy;}
//     inline Idx height() const {
//       return freeY();}
//     inline Idx free() const {return fx+O::width()*fy;}
//     // use space ----
//     inline void useX(Idx ux=1) {if (fx) fx-=ux; else useY();}
//     inline void useY(Idx uy=1) {
//       if (!fy) {
//         fx=0;
//         // fy=0;
//       } else {
//         fy-=uy;
//         fx=O::width();
//       }
//     }
//   protected:
//     inline void _nl(RawOutOp) {O::nl();}
//     inline void _nl(MeasureOp) {O::useY();}
//     template<typename T>
//     inline void _raw(T i,RawOutOp) {O::raw(i);}
//     template<typename T>
//     inline void _raw(T i,MeasureOp) {O::measure(i);}
//     Idx fx,fy;
// };
