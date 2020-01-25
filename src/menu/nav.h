/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I,Idx max_depth>
struct Nav {
  template<typename N>
  struct As:N {
    using This=Nav<I,max_depth>::As<N>;
    using Data=I;
    Idx path[max_depth]{0};
    Idx level=0;
    Data& root;
    Modes editMode;
    inline As(Data& o):root(o){}
    template<typename Out>
    inline void print(Out& out) {root.printMenu(N::obj(),out);}
    inline Idx pos() const {return path[level];}
    inline Modes mode() const {return editMode;}
    inline bool selected(Idx i) const {return path[level]==i;}
  };
};

template<Expr... I>
struct StaticNavRoot:Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>> {
  using Base=typename Chain<I...,Drift>::template To<Obj<StaticNavRoot<I...>>>;
  using This=StaticNavRoot<I...>;
  using Base::Base;
};

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
