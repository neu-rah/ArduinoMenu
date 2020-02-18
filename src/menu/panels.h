/* -*- C++ -*- */
#pragma once

/**************
This os part of the output system
*/

#include "api.h"

namespace Menu {
  ////////////////////////////////////////////////////////////////////////////////
  // panels and viewports
  template<Idx x,Idx y,Idx w,Idx h>
  struct StaticPanel {
    template<typename O>
    struct Part:O {
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
      static inline void use(Idx ux=1,Idx uy=1) {}
    };
  };

  struct PanelTarget {
    template<typename O>
    struct Part:O {
      void *target=nullptr;
      inline bool isSame(void *menu) const {return target==menu;}
      inline void lastDrawn(void *menu) {target=menu;}
    };
  };

  template<int w=1,int h=1>
  struct RangePanel {
    template<typename O>
    class Part:public O {
      public:
        using This=RangePanel<w,h>::Part<O>;
        inline Idx top() const {return topLine;}
        inline void setTop(Idx n) {topLine=n;}
        inline void newView() {
          trace(MDO<<"RangePanel::newView---------------------------"<<endl);
          freeLines=O::height();
          O::setCursor(O::orgX(),O::orgY()+O::ascent());
        }
        inline void useY(Idx uy=h) {
          trace(MDO<<"RangePanel::useY "<<uy<<endl);
          if (freeLines) {
            freeLines-=uy;
          }
        }
        // inline void use(Idx ux=1,Idx uy=1) {This::useX(ux);This::useY(uy);}
        inline void use(Area a) {This::useX(a.height);This::useY(a.width);}
        template<bool toPrint=true>
        inline void nl() {
          trace(MDO<<"RangePanel::nl<"<<toPrint<<">"<<endl);
          O::template nl<toPrint>();
          useY();
        }
        inline Idx freeY() const {return freeLines;}
        inline Idx posY() const {return O::height()-freeLines;}
        inline Idx free() const {return w*freeY();}
        template<typename Nav>
        inline bool posTop(Nav& nav) {
          trace(MDO<<"RangePanel::posTop for "<<nav.pos()<<endl);
          Idx ot=top();
          while(top()>nav.pos()) setTop(top()-1);
          //TODO: this is NOT correct for multiline options!!!!
          while(nav.pos()>=top()+freeY()) setTop(top()+1);
          trace(MDO<<"top:"<<top()<<endl);
          return ot!=top();
        }
      protected:
        Idx topLine=0;//this refers to menu items
        Idx freeLines;//this refers to device free space
    };
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
}
