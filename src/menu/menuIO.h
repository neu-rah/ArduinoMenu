/* -*- C++ -*- */
#pragma once

#include "api.h"

template<Expr... O>
struct MenuOut:Chain<O...,Void>::template To<Obj<MenuOut<O...>>> {
  using Base=typename Chain<O...,Void>::template To<Obj<MenuOut<O...>>>;
  using This=MenuOut<O...>;
  using Base::Base;
};

template<typename O>
struct CanMeasure:O {
  // using O::raw;
  // template<typename T,typename Out,bool toPrint>
  // inline void print(T o,Out& out,Roles role=Roles::Raw) {
  //   if (toPrint) O::template print<T,Out,toPrint>(o,out,role);
  //   else out.useX(O::measure(o));
  // }
};

template<typename O>
struct PartialDraw:O {
  inline static constexpr bool partialDraw() {return true;}
};

template<typename Dev,Dev& dev>
struct StreamOut {
  template<typename O>
  struct As:O {
    using This=StreamOut<Dev,dev>::As<O>;
    inline static void nl() {dev<<endl;}
    template<typename T> inline static void raw(T o) {dev<<o;}
  };
  template<typename O>
  using Template=As<O>;
};

template<typename Dev,Dev& dev,typename O>
struct StreamIn:O {
  template<bool invY=false>
  inline static Cmd cmd() {
    char c;
    dev>>c;
    switch(c) {
      case '+': return Cmd::Up;
      case '-': return Cmd::Down;
      case '*': return Cmd::Enter;
      case '/': return Cmd::Esc;
      default:return Cmd::None;
    }
  }
};
