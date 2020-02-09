/* -*- C++ -*- */
#pragma once

#include "nav.h"

struct INav {
  virtual inline void print(IOut& out)=0;
  virtual inline Idx pos() const=0;
  virtual inline Modes mode() const=0;
  virtual inline bool selected(Idx i) const =0;
};

template<Expr... N>
struct NavRoot:INav,Chain<N...,Drift>::template To<Obj<NavRoot<N...>>> {
  using Base=typename Chain<N...,Drift>::template To<Obj<NavRoot<N...>>>;
  using This=NavRoot<N...>;
  using Base::Base;
  inline void print(IOut& out) override {Base::print(out);}
  inline Idx pos() const override {return Base::pos();}
  inline Modes mode() const override {return Base::mode();}
  inline bool selected(Idx i) const override {return Base::selected(i);}
};
