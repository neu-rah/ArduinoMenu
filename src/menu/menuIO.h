/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Dev,Dev& dev,typename O=Void>
struct StreamOut:O {
  inline static void nl() {dev<<endl;}
  template<typename T>
  inline static void raw(T o) {dev<<o;}
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
