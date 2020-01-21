/* -*- C++ -*- */
#pragma once

#include "api.h"

template<Expr... I>
struct NavRoot:Chain<I...,Drift>::template To<Obj<NavRoot<I...>>> {
  using Base=typename Chain<I...,Drift>::template To<Obj<NavRoot<I...>>>;
  using This=NavRoot<I...>;
  using Base::Base;
};
