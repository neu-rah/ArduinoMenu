/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu navigation implementations
*/

#include "base.h"

template<typename O=Nil> struct Drift:public O {
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  template<typename Nav> constexpr static inline bool _up(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _down(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _left(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _right(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _enter(Nav& nav) {return false;}
  template<typename Nav> constexpr static inline bool _esc(Nav& nav) {return false;}
  // template<typename Nav> constexpr static inline bool up(Nav& nav) {return nav._up(nav);}
  // template<typename Nav> constexpr static inline bool down(Nav& nav) {return nav._down(nav);}
  // template<typename Nav> constexpr static inline bool left(Nav& nav) {return nav._left(nav);}
  // template<typename Nav> constexpr static inline bool right(Nav& nav) {return nav._right(nav);}
  // template<typename Nav> constexpr static inline bool enter(Nav& nav) {return nav._enter(nav);}
  // template<typename Nav> constexpr static inline bool esc(Nav& nav) {return nav._esc(nav);}
};

template<typename Out,typename O=Drift<>>
class NavBase:public O {
  public:
    inline void enterMenuRender() {onMenu=true;}
    inline void exitMenuRender() {onMenu=false;}
    inline idx_t top() const {return out.top();}
    inline void setTop(idx_t n) {out.setTop(n);}
    inline idx_t height() const {return out.height();}
    inline void setFocus(Item*i=NULL) {focus=i;}
    inline void clearFocus() {setFocus();}
    inline Item& getFocus() const {return *focus;}
    inline bool hasFocus() {return focus!=NULL;}
  protected:
    Out out;
    bool onMenu=false;
    Item* focus=NULL;
};

template<typename Out,typename Data,typename O=Drift<>>
class StaticNav:public NavBase<Out,O> {
  public:
    using Base=NavBase<Out,O>;
    using This=StaticNav<Out,Data,O>;
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      Base::enterMenuRender();
      Base::out.newView();
      Base::out.template printMenu<This,Out,Data>(*this,Base::out,data);
      Base::exitMenuRender();
    }
    inline idx_t size() {return data.size();}
    inline void enable(idx_t n,bool o) {data.enable(n,o);}
    inline bool enabled(idx_t n) {return data.enabled(n);}
    // inline Item& operator[](idx_t n) {return data.operator[](n);}
    inline bool activate() {return data.activate();}
    inline bool activate(idx_t n) {return data.activate(n);}
  protected:
    Data data;
};

template<typename Out,typename Data,typename O=Drift<>>
class DynamicNav:public NavNode,public NavBase<Out,O> {
  public:
    using Base=NavBase<Out,O>;
    using This=DynamicNav<Out,Data,O>;
    DynamicNav(Data& o):data(&o) {}
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      Base::enterMenuRender();
      Base::out.newView();
      Base::out.template printMenu<This,Out,Data>(*this,Base::out,*data);
      Base::exitMenuRender();
    }
    inline idx_t size() {return data->size();}
    inline bool selected(idx_t i) const override {return O::selected(i);}
    inline bool enabled(idx_t i) const override {return data->enabled(i);}
    inline void enable(idx_t n,bool o) {data->enable(n,o);}
    inline bool up() override {return O::template _up<This>(*this);}
    inline bool down() override {return O::template _down<This>(*this);}
    inline bool left() override {return O::template _left<This>(*this);}
    inline bool right() override {return O::template _right<This>(*this);}
    inline bool enter() override {return O::template _enter<This>(*this);}
    inline bool esc() override {return O::template _esc<This>(*this);}
    // inline Item& operator[](idx_t n) {return data->operator[](n);}
    inline bool activate() {return data->activate();}
    inline bool activate(idx_t n) {return data->activate(n);}
  protected:
    Data* data;
};

template<typename O=Drift<>>
class NavPos:public O {
  public:
    using O::O;
    using This=NavPos<O>;
    inline bool selected(idx_t idx) const {return at==idx;}
    template<typename Nav>
    inline bool _up(Nav& nav) {
      if (at<nav.size()-1) {at++;return true;}
      return O::template _up<Nav>(nav);
    }
    template<typename Nav>
    inline bool _down(Nav& nav) {
      if (at>0) {at--;return true;}
      return O::template _down<Nav>(nav);
    }
    inline idx_t pos() {return at;}
    template<typename Nav>
    inline bool _enter(Nav& nav) {
      trace(MDO<<"NavPos::_enter "<<at<<endl);
      if (nav.hasFocus()) nav.clearFocus();
      else if (nav.enabled(at)&&nav.activate()) nav.setFocus(&nav.operator[](at));
      else return false;
      return true;
    }
  protected:
    idx_t at;
};

template<typename O>
struct NavCap:public O {
  using This=NavCap<O>;
  inline bool up() {return O::template _up<This>(*this);}
  inline bool down() {return O::template _down<This>(*this);}
  inline bool left() {return O::template _left<This>(*this);}
  inline bool right() {return O::template _right<This>(*this);}
  inline bool enter() {
    trace(MDO<<"NavCap::enter"<<endl);
    return O::template _enter<This>(*this);}
  inline bool esc() {return O::template _esc<This>(*this);}
  // inline bool activate() {return O::activate();}
};
