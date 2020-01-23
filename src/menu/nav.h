/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename I,Idx max_depth>
struct Nav {
  template<typename N>
  struct As:N {
    using This=Nav<I,max_depth>::As<N>;
    using Data=I;
    Idx path[]{0};
    Idx level=0;
    Data& root;
    inline As(Data& o):root(o){}
    template<typename Out>
    inline void print(Out& out) {root.printMenu(N::obj(),out);}
    inline Idx pos() const {return path[level];}
  };
};

template<Expr... I>
struct NavRoot:Chain<I...,Drift>::template To<Obj<NavRoot<I...>>> {
  using Base=typename Chain<I...,Drift>::template To<Obj<NavRoot<I...>>>;
  using This=NavRoot<I...>;
  using Base::Base;
};
