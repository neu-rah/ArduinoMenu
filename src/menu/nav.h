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
};

template<typename Out,typename Data,typename O=Drift<>>
class StaticNav:public O {
  public:
    using This=StaticNav<Out,Data,O>;
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.template printMenu<This,Out,Data>(*this,out,data);
    }
    inline idx_t size() {return data.size();}
  protected:
    Out out;
    Data data;
};

template<typename Out,typename Data,typename O=Drift<>>
class DynamicNav:public NavNode,public O {
  public:
    using This=DynamicNav<Out,Data,O>;
    DynamicNav(Data& o):data(&o) {}
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.template printMenu<This,Out,Data>(*this,out,*data);
    }
    inline idx_t size() {return data->size();}
    inline bool selected(idx_t i) const override {return O::selected(i);}
    inline bool enabled(idx_t i) const override {return O::enabled(i);}
    inline bool up() override {return O::template _up<This>(*this);}
    inline bool down() override {return O::template _down<This>(*this);}
    inline bool left() override {return O::template _left<This>(*this);}
    inline bool right() override {return O::template _right<This>(*this);}
    inline bool enter() override {return O::template _enter<This>(*this);}
    inline bool esc() override {return O::template _esc<This>(*this);}
  protected:
    Out out;
    Data* data;
};

template<typename O=Drift<>>
class NavPos:public O {
  public:
    using O::O;
    using This=NavPos<O>;
    // template<idx_t idx>
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
  inline bool enter() {return O::template _enter<This>(*this);}
  inline bool esc() {return O::template _esc<This>(*this);}
};
