/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <stdint.h>
#else
  #include <limits>
#endif

#include "base.h"

namespace AM5 {
  //////////////////////////////////////////////////////
  // output
  template<typename O=Nil>
  struct Void:public O {
    constexpr static inline bool isRange() {return false;}
    constexpr static inline bool isViewport() {return false;}
    constexpr static inline size_t top() {return 0;}
    static inline void setTop(size_t) {}
    static inline void newView() {}
    constexpr static inline idx_t posX() {return 0;}
    constexpr static inline idx_t posY() {return 0;}
    constexpr static inline idx_t freeX() {return INT16_MAX;}
    constexpr static inline idx_t freeY() {return INT16_MAX;}
    constexpr static inline idx_t free() {return INT16_MAX;}
    static inline void useX(idx_t ux=1) {}
    static inline void useY(idx_t uy=1) {}
    template<typename,typename,bool,size_t> static inline void fmtPanel() {}
    template<typename,typename,bool,size_t> static inline void fmtMenu() {}
    template<typename,typename,bool,size_t> static inline void fmtMenuBody() {}
    template<typename,typename,bool,size_t> static inline void fmtTitle() {}
    template<typename,typename,bool,size_t> static inline void fmtItem() {}
    template<typename,typename,bool,size_t> static inline void fmtIndex() {}
    template<typename,typename,bool,size_t> static inline void fmtCursor() {}
    template<typename T> static inline void raw(T) {}
    static inline void nl() {}
    // static inline bool onMenuRender() {return false;}
    // static inline void enterMenuRender() {}
    // static inline void exitMenuRender() {}
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

    constexpr static inline idx_t posX() {return x;}
    constexpr static inline idx_t posY() {return y;}
    constexpr static inline idx_t freeX() {return w;}
    constexpr static inline idx_t freeY() {return h;}
    constexpr static inline idx_t free() {return w*h;}
    static inline void useX(idx_t ux=1) {}
    static inline void useY(idx_t uy=1) {}
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
      inline Viewport() {/*newView();*/}
      inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
      constexpr static inline bool isViewport() {return true;}
      inline operator bool() const {return fx&&fy;}
      inline operator int() const {return free();}
      inline void newView() {fx=O::width();fy=O::height();}
      //TODO: need font size and char measure API
      inline void nl() {useY(1);}
      //device coordinates ---------
      inline idx_t posX() const {return (O::width()-fx)+O::orgX();}
      inline idx_t posY() const {return (O::height()-fy)+O::orgY();}
      // get free space ----
      inline idx_t freeX() const {return fx;}
      inline idx_t freeY() const {
        // Serial<<"Viewport::freeY "<<fy<<endl;
        return fy;}
      inline size_t height() const {
        return freeY();}
      inline idx_t free() const {return fx+O::width()*fy;}
      // use space ----
      inline void useX(idx_t ux=1) {if (fx) fx-=ux; else useY();}
      inline void useY(idx_t uy=1) {
        // Serial<<"Viewport::useY("<<uy<<")"<<endl;
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

  template<typename O,typename... OO>
  class OutList:public O {
    public:
      using This=OutList<O,OO...>;
      using O::O;
      //this works because printer head is never taken at this level
      //so dont do it!
      inline void newView() {
        Serial<<"OutList::newView"<<endl;
        O::newView();
        if (!This::onMenuRender()) next.newView();
      }
      template<typename Nav,typename T>
      inline void raw(T o) {
        O::template raw<Nav,T>(o);
        //without this global print hits only the first device
        //with it menus will chain printing to next devices creating chaos
        if (!This::onMenuRender()) next.template raw<Nav,T>(o);//chain printing to all devices!
      }
      template<typename Nav>
      inline void printMenu() {
        Serial<<"OutList::printMenu"<<endl;
        assert(onMenuRender());
        O::template printMenu<Nav>();
        Serial<<"..."<<endl;
        next.newView();
        next.template printMenu<Nav>();
      }
      // template<typename P>
      // inline void printMenuRaw(MenuOut& menuOut,P p,Item&i) {
      //   assert(O::onMenuRender());
      //   O::printMenuRaw(menuOut,p,i);
      //   next.newView();
      //   next.printMenuRaw(next,PrintHead<OutList<OO...>>{/*next,*/next,0},i);
      // }
      // inline void setTarget(Item& i) {
      //   O::setTarget(i);
      //   assert(O::sharedNav());
      //   //next.setTarget(i);
      // }
      static inline bool onMenuRender() {return onMenu;}
      static inline void enterMenuRender() {onMenu=true;}
      static inline void exitMenuRender() {onMenu=false;}
    protected:
      static bool onMenu;
      OutList<OO...> next;
  };

  template<typename O>
  struct OutList<O>:public O {using O::O;};



  ///////////////////////////////////////////////////////////////////
  // output interface
  class MenuOut {
    // friend class Void;
    public:
      virtual inline void raw(const char* o) {}
      // static inline void nl() {device<<std::endl;}
      // virtual MenuOut& operator<<(Item&) {return *this;}
      // virtual MenuOut& operator<<(const char*) {return *this;}
      // virtual MenuOut& operator<<(char) {return *this;}
      // virtual MenuOut& operator<<(unsigned char) {return *this;}
      // virtual MenuOut& operator<<(int) {return *this;}
      // virtual MenuOut& operator<<(unsigned int) {return *this;}
      // #if defined(ARDUINO) && !defined(__AVR_ATtiny13__) && !defined(ATTINY_CORE)
      //   virtual MenuOut& operator<<(endlObj) {return *this;}
      //   virtual MenuOut& operator<<(const __FlashStringHelper *i) {return *this;}
      // #endif
      // virtual void fmtMenu(bool io)=0;
      // virtual void fmtPanel(bool io)=0;
      // virtual void fmtTitle(bool io)=0;
      // virtual void fmtItem(bool io)=0;
      // virtual void fmtAccel(bool io)=0;
      // virtual void fmtCursor(bool io)=0;
      // virtual void fmtLabel(bool io)=0;
      // virtual void fmtMode(bool io)=0;
      // virtual void fmtValue(bool io)=0;
      // virtual void fmtUnit(bool io)=0;
      // virtual void printMenuRaw()=0;
      // virtual void setTarget(Item& i)=0;
      // virtual Item& getTarget() {return *(Item*)NULL;}
    protected:
      // static bool onMenu;
    };

};
