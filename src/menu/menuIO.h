/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename O>
struct CanMeasure:O {
  using O::nl;
  template<typename Out,bool toPrint=true>
  inline void nl(Out& out) {
    if (toPrint) out.nl();
    else out.useY();
  }
  using O::raw;
  template<typename T,typename Out,bool toPrint>
  inline void raw(T o,Out& out,Roles role=Roles::Raw) {
    if (toPrint) out.raw(o,role);
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
  // template<typename Out=This,bool toPrint=true>
  // inline static void nl(Out& out) {
  //   if (toPrint) nl();
  //   else out.useY();
  // }

  template<typename T>
  inline static void raw(T o,Roles role=Roles::Raw) {dev<<o;}
  // template<typename T,typename Out,bool toPrint=true>
  // inline static void raw(T o,Out& out,Roles role=Roles::Raw) {
  //   if (toPrint) dev<<o;
  //   else out.use(O::measure(o),1);//assuming only one line internal string \n are not accounted for
  // }
  // template<typename T>
  // inline static void raw(T o,Roles role=Roles::Raw) {
  //   if (toPrint) dev<<o;
  //   else out.use(measure(o),1);//assuming only one line internal string \n are not accounted for
  // }
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
