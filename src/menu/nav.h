/* -*- C++ -*- */
#pragma once
/**
* @file nav.h
* @author Rui Azevedo
* @brief ArduinoMenu navigation implementations
*/

/** \defgroup Navigation Navigation system
 *  @{
 */

 #include "base.h"

/*
* The Drift class is the terminal navigation element
*/
template<typename N=Nil>
struct Drift:public N {
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline Modes mode() {return Modes::Normal;}
  constexpr static inline bool hasFocus() {return false;}
  template<typename Nav> constexpr static inline Modes _mode(Nav&) {return Modes::Normal;}
  template<typename Nav> constexpr static inline Modes mode(Nav& nav) {return nav._mode();}
  template<typename Nav> constexpr static inline bool _up(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _down(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _left(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _esc(Nav& nav) {return false;}
};

/**
* The NavBase class. Provides common navigation functionality
* @brief this is a brief
*/
template<typename Out,typename Data,typename N=Drift<>>
class NavBase:public N {
  public:
    using This=NavBase<Out,Data,N>;
    using OutType=Out;
    using DataType=Data;
    NavBase():data(NULL) {}
    NavBase(Data& o):data(&o) {}
    inline void enterMenuRender() {onMenu=true;}
    inline void exitMenuRender() {onMenu=false;}
    inline idx_t top() const {return out.top();}
    inline void setTop(idx_t n) {out.setTop(n);}
    inline idx_t height() const {return out.height();}
    inline void setTarget(Data& d) {data=&d;}
    template<typename Nav>
    inline void printMenu(Nav& nav) {
      out.template printMenu<Nav,Out,Data>(nav,out,*data);
    }
    inline idx_t size() {return data->size();}
    inline void enable(idx_t n,bool o) {data->enable(n,o);}
    inline bool enabled(idx_t n) {return data->enabled(n);}
    inline NavAgent activate() {return data->activate();}
    inline NavAgent activate(idx_t n) {return data->activateItem(n);}
    template<typename Nav>
    inline Modes _mode(Nav&) const {return data->mode();}
    inline void newView() {out.newView();}
  protected:
    Out out;
    bool onMenu=false;
    Data* data;
};

/*
* The NavRoot class encapsulates navigation components and implements/redirects navigation interface
* use dynamic nav instead
*/
template<typename N>
struct NavRoot:public N {
  using N::N;
  using This=NavRoot<N>;
  inline bool up() {return N::template _up<This>(*this);}
  inline bool down() {return N::template _down<This>(*this);}
  inline bool left() {return N::template _left<This>(*this);}
  inline bool right() {return N::template _right<This>(*this);}
  inline bool enter() {
    trace(MDO<<"NavRoot::enter"<<endl);
    return N::template _enter<This>(*this);}
  inline bool esc() {return N::template _esc<This>(*this);}
  inline Modes mode() {return N::_mode(*this);}
    // _trace(MDO<<"NavRoot::mode"<<endl);
    // return N::template _mode<This>(*this);}
  // inline NavAgent activate() {return N::activate();}
  inline void printMenu() {
    N::enterMenuRender();
    N::newView();
    N::template printMenu<This>(*this);
    N::exitMenuRender();
  }
};

/**
* The DynamicNav class. Can point o other target menu
*/
template<typename Out,typename Data,typename N=Drift<>>
class DynamicNav:public NavNode,public NavRoot<NavBase<Out,Data,N>> {
  public:
    using Base=NavRoot<NavBase<Out,Data,N>>;
    using This=DynamicNav<Out,Data,N>;
    // DynamicNav(Data& o):data(&o) {}
    // inline void printMenu() {This::printMenu(*this);}
    // template<typename Nav>
    // inline void printMenu(Nav& nav) {
    //   Base::out.template printMenu<Nav,Out,Data>(nav,Base::out,*Base::data);
    // }
    // inline bool selected(idx_t i) const override {return N::selected(i);}
    // inline bool enabled(idx_t i) const override {return data->enabled(i);}
    // inline void enable(idx_t n,bool o) {data->enable(n,o);}
    inline bool up() override {return N::template _up<This>(*this);}
    inline bool down() override {return N::template _down<This>(*this);}
    inline bool left() override {return N::template _left<This>(*this);}
    inline bool right() override {return N::template _right<This>(*this);}
    inline bool enter() override {return N::template _enter<This>(*this);}
    inline bool esc() override {return N::template _esc<This>(*this);}
    // inline Item& operator[](idx_t n) {return data->operator[](n);}
    // inline NavAgent activate() {return data->activate();}
    // inline NavAgent activate(idx_t n) {return data->activateItem(n);}
    // inline Modes mode() const override {return N::mode();}
/*  protected:
    Data* data;*/
};

/**
* The NavPos class holds selected item index, is a primitive navigation functionality
*/
template<typename N=Drift<>>
class NavPos:public N {
  public:
    using N::N;
    using This=NavPos<N>;
    inline bool selected(idx_t idx) const {return at==idx;}
    template<typename Nav>
    inline bool _up(Nav& nav) {
      if (at<nav.size()-1) {at++;return true;}
      return N::template _up<Nav>(nav);
    }
    template<typename Nav>
    inline bool _down(Nav& nav) {
      if (at>0) {at--;return true;}
      return N::template _down<Nav>(nav);
    }
    inline idx_t pos() {return at;}
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      trace(MDO<<"ItemNav::_enter"<<endl);
      return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::_enter(nav));
    }
    // template<typename Nav>
    // inline bool _enter(Nav& nav) {
    //   _trace(MDO<<"NavPos::_enter "<<at<<endl);
    //   // if (nav.hasFocus()) nav.clearFocus();
    //   // else if (nav.enabled(at)&&nav.activate()) nav.setFocus(&nav.operator[](at));
    //   // else return false;
    //   // return true;
    //   // return false;
    // }
  protected:
    idx_t at;
};

/**
* The ItemNav class allow items to handle navigation (needed for fields)
* items can handle up|down|enter|esc
* left|right are a thing of the navigation system that can steal
* focus from the field, an enter is sent to the field instead, to validate the entry
*/
template<typename N>
class ItemNav:public N {
  public:
    using N::N;
    template<typename Nav>
    inline bool _down(Nav& nav) {
      return focus?focus.down():N::_down(nav);
    }
    template<typename Nav>
    inline bool _up(Nav& nav) {
      return focus?focus.up():N::_up(nav);
    }
    template<typename Nav>
    inline bool _left(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::_left(nav);
    }
    template<typename Nav>
    inline bool _right(Nav& nav) {
      if (focus) {
        focus.enter();
        focus=Empty<>::activate();
      }
      return N::_right(nav);
    }
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      if (focus) {
        if (focus.enter()) return true;
        focus=Empty<>::activate();//blur if enter return false
      } else {
        if (!nav.enabled(nav.pos())) return false;
        focus=nav.activate(nav.pos());
        if (focus.result()) return true;
      }
      return false;
    }
    template<typename Nav>
    inline bool _esc(Nav& nav) {
      if (focus) {
        if (focus.esc()) focus=Empty<>::activate();
        return true;
      }
      return N::_esc(nav);
    }
    template<typename Nav>
    inline Modes _mode(Nav& nav) const {
      _trace(MDO<<"ItemNav::_mode"<<endl);
      return focus.result()?focus.mode():Modes::Normal;}
    inline bool hasFocus() const {return focus;}
  protected:
    NavAgent focus;
};

//static nav tree fails here because the same navnode should be re-used for multiple target!
template<typename N,idx_t max_depth=1>
class NavTree {
  public:
    using This=NavTree<N,max_depth>;
    NavTree(typename N::DataType& data) {path[0].setTarget(data);}
    inline bool selected(idx_t n) {return path[level].selected(n);}
    inline Modes mode() {return path[level].mode();}
    inline void enterMenuRender() {path[level].enterMenuRender();}
    inline void exitMenuRender() {path[level].exitMenuRender();}
    inline void newView() {path[level].newView();}
    template<typename Data>
    inline void setTarget(Data d) {path[level].setTarget(d);}
    inline idx_t size() {return path[level].size();}
    inline void enable(idx_t n,bool o) {path[level].enable(n,o);}
    inline bool enabled(idx_t n) {return path[level].enabled(n);}
    inline NavAgent activate() {return path[level].activate();}
    inline NavAgent activate(idx_t n) {return path[level].activate(n);}
    inline bool hasFocus() const {return path[level].hasFocus();}
    // inline Modes _mode() const {return path[level].mode();}
    template<typename Nav>
    inline Modes _mode(Nav& nav) {return path[level]._mode(nav);}

    template<typename Nav>
    inline void printMenu(Nav& nav) {path[level].printMenu(nav);}

    //TODO: remove member access!
    template<typename Nav> inline bool _enter(Nav& nav) {
      bool r=path[level]._enter(nav);
      if (r&&!hasFocus()) level++;
      return r;
    }
    template<typename Nav>
    inline bool _esc(Nav& nav) {
      bool r=path[level]._esc(nav);
      if (!path[level].hasFocus()) level--;
      return r;
    }
    template<typename Nav> inline bool _up(Nav& nav) {return path[level]._up(nav);}
    template<typename Nav> inline bool _down(Nav& nav) {return path[level]._down(nav);}
    template<typename Nav> inline bool _left(Nav& nav) {return path[level]._left(nav);}
    template<typename Nav> inline bool _right(Nav& nav) {return path[level]._right(nav);}
    inline idx_t pos() {return path[level].pos();}
  protected:
    idx_t level=0;
    N path[max_depth];
};

/** @}*/
