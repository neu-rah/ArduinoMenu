/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu output implementations
*/

#include "base.h"

#ifndef ARDUINO
  #include <string>
  using namespace std;
#endif

struct TM {
  template<typename T>
  static inline idx_t measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return string(o).length();
    #endif
  }
};

template<> idx_t TM::measure<const char>(const char o) {return 1;}
// template<> idx_t TM::measure<const char*>(const char* o) {return strlen(o);}

template<typename O=Nil> struct Void:public O {
  static inline void nl() {}
  // template<typename T> static inline void raw(T) {}
  // template<typename Nav,typename Out,typename I>
  // static inline void printMenu(Nav&,Out& out,I& i) {}
  // template<typename Nav,typename Out,typename I>
  // static inline void printItem(Nav&,Out& out,I& i) {i.printItem(out);}

  constexpr static inline bool isRange() {return false;}//TODO: do we need this here?
  constexpr static inline bool isViewport() {return false;}
  constexpr static inline idx_t height() {return 0;}
  constexpr static inline idx_t top() {return 0;}
  static inline void setTop(idx_t) {}
  static inline void newView() {nl();}
  constexpr static inline idx_t posX() {return 0;}
  constexpr static inline idx_t posY() {return 0;}
  constexpr static inline idx_t freeX() {return INT16_MAX;}
  constexpr static inline idx_t freeY() {return INT16_MAX;}
  constexpr static inline idx_t free() {return INT16_MAX;}
  static inline void useX(idx_t ux=1) {}
  static inline void useY(idx_t uy=1) {}

  template<bool io,typename Nav,typename Out,typename I> static inline void fmtPanel(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMenu(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtTitle(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtBody(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtItem(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtIndex(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtCursor(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtName(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtMode(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtValue(Nav&,Out&,I&,idx_t) {}
  template<bool io,typename Nav,typename Out,typename I> static inline void fmtUnit(Nav&,Out&,I&,idx_t) {}

  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,Nav& nav,Out& out,I& i,idx_t n) {
    fmt(role,true,nav,out,i,n);
    fmt(role,false,nav,out,i,n);
  }
  template<typename Nav,typename Out,typename I>
  static inline void fmt(Roles role,bool io,Nav& nav,Out& out,I& i,idx_t n) {
    //we could do better with templates, but we need this to be compatible with virtual interface too
    switch(role) {
      case Roles::Panel: io?out.template fmtPanel<true>(nav,out,i,n):out.template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?out.template fmtMenu<true>(nav,out,i,n):out.template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?out.template fmtTitle<true>(nav,out,i,n):out.template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?out.template fmtBody<true>(nav,out,i,n):out.template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?out.template fmtItem<true>(nav,out,i,n):out.template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?out.template fmtIndex<true>(nav,out,i,n):out.template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?out.template fmtCursor<true>(nav,out,i,n):out.template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?out.template fmtName<true>(nav,out,i,n):out.template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?out.template fmtMode<true>(nav,out,i,n):out.template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?out.template fmtValue<true>(nav,out,i,n):out.template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?out.template fmtUnit<true>(nav,out,i,n):out.template fmtUnit<false>(nav,out,i,n);break;
    }
  }

};

//static output -----------------------------
template<typename O=Void<TM>>
struct FullPrinter:public O {
  template<typename Nav,typename Out,typename I>
  inline void printMenu(Nav& nav,Out& out,I& i) {
    trace(MDO<<"FullPrinter::printMenu"<<endl);
    out.template fmtPanel<true>(nav,out,i,0);
    out.template fmtMenu<true>(nav,out,i,0);
    out.template fmtTitle<true>(nav,out,i,0);
    i.print(nav,out);
    out.template fmtTitle<false>(nav,out,i,0);
    out.template fmtBody<true>(nav,out,i,0);

    if (Out::isRange()) {
      //ensure that selection option is withing range
      while(out.top()+out.posY()>=nav.pos())
        out.setTop(out.top()-1);
      while(nav.pos()>=out.top()+out.freeY())
        out.setTop(out.top()+1);
    }

    for(idx_t n=out.top();n<i.size();n++) {
      out.template fmtItem<true>(nav,out,*this,n);
      out.template fmtIndex<true>(nav,out,*this,n);
      out.template fmtIndex<false>(nav,out,*this,n);
      out.template fmtCursor<true>(nav,out,*this,n);
      out.template fmtCursor<false>(nav,out,*this,n);
      i.printItem(nav,out,n);
      out.template fmtItem<false>(nav,out,*this,n);
    }

    out.template fmtBody<false>(nav,out,i,0);
    out.template fmtMenu<false>(nav,out,i,0);
    out.template fmtPanel<false>(nav,out,i,0);
  }
  template<typename Nav,typename Out,typename I>
  static inline void printItem(Nav& nav,Out& out,I& i,idx_t n) {
    out.template fmtItem<true>(nav,out,i,n);
    out.template fmtIndex<true>(nav,out,i,n);
    out.template fmtIndex<false>(nav,out,i,n);
    out.template fmtCursor<true>(nav,out,i,n);
    out.template fmtCursor<false>(nav,out,i,n);
    i.printItem(nav,out,n);
    out.template fmtItem<false>(nav,out,i,n);
  }
};

//static panel ------------------------------------------------
// describes output geometry,
// may be whole device, but must not exceed
// it has origin coordinates to be displaced around
template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
struct _StaticPanel:public O {
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

template<idx_t x,idx_t y,idx_t w,idx_t h,typename O>
struct StaticPanel:public _StaticPanel<x,y,w,h,O> {};

template<idx_t w,idx_t h,typename O>
struct StaticPanel<0,0,w,h,O>:public _StaticPanel<0,0,w,h,O> {};

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
    inline Viewport() {/*newView();*/}
    inline Viewport(const Viewport<O>& o) {fx=o.width();fy=o.height();}
    constexpr static inline bool isViewport() {return true;}
    inline operator bool() const {return fx&&fy;}
    inline operator int() const {return free();}
    inline void newView() {
      trace(MDO<<"newView()"<<endl);
      fx=O::width();fy=O::height();//+O::top();
      //O::newView();
    }
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
    inline idx_t height() const {
      return freeY();}
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

template<typename Dev,Dev dev,typename O=FullPrinter<>>
struct RawOut:public O {
  template<typename T> static inline void raw(T o) {dev<<o;}
};

template<typename O,typename... OO>
class OutList:public OutList<O> {
  public:
    using This=OutList<O,OO...>;
    using O::O;
    //this works because printer head is never taken at this level
    //so dont do it!
    inline void newView() {
      // Serial<<"OutList::newView "<<This::onMenuRender()<<endl;
      O::newView();
      // if (Nav::onMenuRender()) next.newView();
    }
    inline void nl() {
      O::nl();
      next.nl();
    }
    template<typename Nav,typename T>
    inline void raw(T o) {
      O::template raw<Nav,T>(o);
      //without this global print hits only the first device
      //with it menus will chain printing to next devices creating chaos
      if (!Nav::onMenuRender()) next.template raw<Nav,T>(o);//chain printing to all devices!
    }
    template<typename Nav,typename Raw>
    inline void printMenu() {
      // Serial<<"..."<<This::onMenuRender()<<endl;
      OutList<O>::template printMenu<Nav,Raw>();
      next.newView();
      // Serial<<"..."<<endl;
      next.template printMenu<Nav,Raw>();
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
  protected:
    OutList<OO...> next;
};

template<typename O>
class OutList<O>:public O {
  public:
    using O::O;
    template<typename Nav,typename Raw>
    inline void printMenu() {
      // Serial<<"OutList<O>::printMenu()"<<Nav::onMenuRender()<<endl;
      // assert(onMenuRender());
      O::template printMenu<Nav,Raw>();
    }
};

//dynamic output --------------------------------
template<typename O>
struct MenuOutDef:public O,public MenuOut {
  inline void nl() override {O::nl();}
  inline void raw(char c) override {O::raw(c);}
  inline void raw(const char*text) override {O::raw(text);}
  inline void raw(int n) override {O::raw(n);}
  virtual inline void raw(unsigned int n) {O::raw(n);}
  virtual inline void raw(long n) {O::raw(n);}
  virtual inline void raw(unsigned long n) {O::raw(n);}
  virtual inline void raw(double n) {O::raw(n);}
  inline void printItem(NavNode& nav,Item& i,idx_t n) override {i.printItem(nav,*this,n);}
  inline void fmt(Roles role,bool io,NavNode& nav,MenuOut& out,Item& i,idx_t n) override {
    switch(role) {
      case Roles::Panel: io?O::template fmtPanel<true>(nav,out,i,n):O::template fmtPanel<false>(nav,out,i,n);break;
      case Roles::Menu: io?O::template fmtMenu<true>(nav,out,i,n):O::template fmtMenu<false>(nav,out,i,n);break;
      case Roles::Title: io?O::template fmtTitle<true>(nav,out,i,n):O::template fmtTitle<false>(nav,out,i,n);break;
      case Roles::Body: io?O::template fmtBody<true>(nav,out,i,n):O::template fmtBody<false>(nav,out,i,n);break;
      case Roles::Item: io?O::template fmtItem<true>(nav,out,i,n):O::template fmtItem<false>(nav,out,i,n);break;
      case Roles::Index: io?O::template fmtIndex<true>(nav,out,i,n):O::template fmtIndex<false>(nav,out,i,n);break;
      case Roles::Cursor: io?O::template fmtCursor<true>(nav,out,i,n):O::template fmtCursor<false>(nav,out,i,n);break;
      case Roles::Name: io?O::template fmtName<true>(nav,out,i,n):O::template fmtName<false>(nav,out,i,n);break;
      case Roles::Mode: io?O::template fmtMode<true>(nav,out,i,n):O::template fmtMode<false>(nav,out,i,n);break;
      case Roles::Value: io?O::template fmtValue<true>(nav,out,i,n):O::template fmtValue<false>(nav,out,i,n);break;
      case Roles::Unit: io?O::template fmtUnit<true>(nav,out,i,n):O::template fmtUnit<false>(nav,out,i,n);break;
    }
  }
};
