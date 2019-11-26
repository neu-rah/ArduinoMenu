/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename O>
struct CanMeasure:O {
  using O::raw;
  template<typename T,typename Out,bool toPrint>
  inline void print(T o,Out& out,Roles role=Roles::Raw) {
    if (toPrint) O::template print<T,Out,toPrint>(o,out,role);
    else out.useX(O::measure(o));
  }
};

template<typename O>
struct PartialDraw:O {
  inline static constexpr bool partialDraw() {return true;}
};

template<typename Dev,Dev& dev,typename O=TextMeasure>
struct StreamOut:O {
  using This=StreamOut<Dev,dev,O>;

  inline static void nl() {dev<<endl;}
  using O::raw;
  template<typename T>
  inline static void raw(T o,Roles=Roles::Raw) {dev<<o;}
};

template<typename Dev,Dev& dev,typename O=Void>
struct StreamIn:O {
  template<typename Nav,bool invY=false>
  inline static bool cmd(Nav& nav) {
    char c;
    dev>>c;
    switch(c) {
      case '+': nav.up();break;
      case '-': nav.down();break;
      case '*': nav.enter();break;
      case '/': nav.esc();break;
      default:return false;
    }
    return true;
  }
};
