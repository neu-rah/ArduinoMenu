/* -*- C++ -*- */
#pragma once

/**************
This os part of the output system
*/

#include "kernel/api.h"

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
        inline Idx top() const {return topItem;}
        inline void setTop(Idx n) {topItem=n;}
        inline void newView() {
          trace(MDO<<"RangePanel::newView---------------------------"<<endl);
          freeLines=O::height();
          O::setCursor(O::orgX(),O::orgY()+O::ascent());
        }
        inline void useY(Idx uy=h) {
          trace(MDO<<"RangePanel::useY "<<uy<<endl);
          if (freeLines) freeLines-=uy;
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
        Idx topItem=0;//this refers to menu items
        Idx freeLines;//this refers to device free space
    };
  };

  struct Viewport {
    template<typename O>
    struct Part:O {
      using This=Viewport::Part<O>;
      using Base=O;
      inline Part() {fx=O::width();fy=O::height();}
      inline Part(const This& o) {fx=o.width();fy=o.height();}
      // constexpr static inline bool isViewport() {return true;}
      inline operator bool() const {return fx&&fy;}
      inline operator int() const {return O::free();}
      inline Idx top() const {return topItem;}
      inline void setTop(Idx n) {topItem=n;}
      inline void newView() {
        _trace(MDO<<endl<<"Viewport@"<<(long)this<<"::newView freeY:"<<freeY()<<" -------------------------"<<endl);
        fx=O::width();fy=O::height();
      }
      // //device coordinates ---------
      inline Idx posX() const {return O::width()-fx;}
      inline Idx posY() const {return O::height()-fy;}
      // // get free space ----
      inline Idx freeX() const {
        _trace(MDO<<"Viewport@"<<(long)this<<"::freeX "<<fx<<" posX:"<<posX()<<endl);
        return fx;}
      inline Idx freeY() const {
        _trace(MDO<<"Viewport@"<<(long)this<<"::freeY "<<fy<<endl);
        return fy;}
      inline Idx height() const {return freeY();}
      inline Idx free() const {return fx+O::width()*fy;}
      // // use space ----
      inline void useX(Idx ux=1) {
        if (fx) fx-=ux;
        else {
          _trace(MDO<<"useX wrapping!!!!! => useY"<<endl);
          useY();
        }
        _trace(MDO<<"Viewport@"<<(long)this<<"::useX "<<ux<<" freeX:"<<freeX()<<endl);
      }
      inline void useY(Idx uy=1) {
        if (!fy) {
          fx=0;
          // fy=0;
        } else {
          fy-=uy;
          if(uy) fx=O::width();
        }
        _trace(MDO<<"Viewport@"<<(long)this<<"::useY "<<uy<<" freeY:"<<freeY()<<endl);
      }
      template<typename Nav>
      inline bool posTop(Nav& nav) {
        _trace(MDO<<"Viewport@"<<(long)this<<"::posTop for "<<nav.pos()<<endl);
        Idx ot=top();
        while(top()>nav.pos()) setTop(top()-1);
        //TODO: this is NOT correct for multiline options!!!!
        while(nav.pos()>=top()+freeY()) setTop(top()+1);
        _trace(MDO<<"top:"<<top()<<endl);
        return ot!=top();
      }
      template<bool toPrint=true>
      inline void nl() {
        _trace(MDO<<"Viewport@"<<(long)this<<"::nl<"<<toPrint<<">"<<endl);
        O::template nl<toPrint>();
        useY();
      }
      template<typename T,bool toPrint=true>
      inline void raw(T o) {
        _trace(MDO<<"Viewport@"<<(long)this<<"::raw<"<<(toPrint?"true":"false")<<">("<<o<<")"<<endl);
        O::template raw<T,toPrint>(o);
        Area used=O::measure(o);
        useX(used.width);
        useY(used.height-O::maxCharHeight());
      }
    protected:
      Idx fx,fy;
      Idx topItem=0;//this refers to menu items
    };
  };

};
