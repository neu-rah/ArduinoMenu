/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev,typename O=Void>
struct SerialIn:O {
  template<typename Nav,bool invY=false>
  inline static bool cmd(Nav& nav) {
    if (dev.available()) {
      char key=dev.read();
      switch(key) {
        case '+': invY?nav.down():nav.up();break;
        case '-': invY?nav.up():nav.down();break;
        case '*': nav.enter();break;
        case '/': nav.esc();break;
        default:return false;
      }
      return true;
    }
    return false;
  }
};
