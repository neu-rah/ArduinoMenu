/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  //provides access to the top level type and object
  //when processing the chain
  //this is essential for this composition schema to work
  //so essential that it should be a pattern by itseld (TODO)
  template<typename Nav,typename Out,typename T,size_t p>
  struct PrintHead {
    using Printer=Out;
    using Target=T;
    using NavRoot=Nav;
    using This=PrintHead<Nav,Printer,Target,p>;
    Target& item;
    Nav& nav;
    Out& menuOut;
    constexpr static inline size_t pos() {return p;}
    inline bool selected() const {return nav.selected(p);}
    inline bool enabled() const {return nav.template enabled<p>();}
    template<typename H,bool io> inline void fmtItem() {item.template fmtItem<This,io>(*this);}
    template<typename H,bool io> inline void fmtMenu() {item.template fmtMenu<This,io>(*this);}
    template<typename H,bool io> inline void fmtMenuBody() {item.template fmtMenuBody<This,io>(*this);}
    template<typename H,bool io> inline void fmtTitle() {item.template fmtTitle<This,io>(*this);}
    template<typename H,bool io> inline void fmtIndex() {item.template fmtIndex<This,io>(*this);}
    template<typename H,bool io> inline void fmtCursor() {item.template fmtCursor<This,io>(*this);}
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

  // the advantage of using sub-part item is that
  // the user can either ommit (same as ommit the formats)
  // or reorder them, not using sub-printers yet...
  // so we call the fmt functions directly here
  // TODO: instead of having multiple printers
  // make this one depend on panel size
  template<typename O>
  struct FullPrinterDef:public O {
    template<typename Nav>
    inline void printMenu(Nav& nav) {
      using Out=typename Nav::NavOut;
      using Target=typename Nav::NavTarget;
      // Serial<<"full menu printer"<<endl;
      // using This=FullPrinterDef<O>;
      if (nav.isRange()) {
        while(nav.top()>nav.pos())
          nav.setTop(nav.top()-1);
        while(nav.pos()>=nav.top()+nav.height())
          nav.setTop(nav.top()+1);
      }
      using ThisPH=PrintHead<Nav,Out,FullPrinterDef<O>,0>;
      ThisPH ph{*this,nav,nav.getOut()};
      Out::template fmtMenu<ThisPH,true>(ph);
      Out::template fmtMenuBody<ThisPH,true>(ph);
      Out::template fmtTitle<ThisPH,true>(ph);
      Target::template out<ThisPH>(ph);
      Out::template fmtTitle<ThisPH,false>(ph);
      nav.template printItems<ThisPH,0>(ph);
      Out::template fmtMenuBody<ThisPH,false>(ph);
      Out::template fmtMenu<ThisPH,false>(ph);
    }
  };
};
