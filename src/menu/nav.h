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
    inline bool selected(idx_t i) const override {return O::selected(i);}
    inline bool enabled(idx_t i) const override {return O::enabled(i);}
  protected:
    Out out;
    Data* data;
};

template<typename O=Drift<>>
class NavPos:public O {
  public:
    using O::O;
    // template<idx_t idx>
    inline bool selected(idx_t idx) const {return at==idx;}
    template<typename Nav>
    inline bool _up() {
      if (at<Nav::size()-1) {at++;return true;}
      return O::template _up<Nav>();
    }
    template<typename Nav>
    inline bool _down() {
      if (at>0) {at--;return true;}
      return O::template _down<Nav>();
    }
    inline idx_t pos() {return at;}
  protected:
    idx_t at;
};
