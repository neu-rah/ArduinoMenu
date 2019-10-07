/* -*- C++ -*- */
#pragma once

template<typename Dev=decltype(Serial), Dev& dev=Serial,typename O=Void<>>
struct ArduinoStreamIn:O {
  template<typename Nav,bool invY=false>
  inline static bool cmd(Nav& nav) {
    if (dev.available()) {
      switch(dev.read()) {
        case '+': return invY?nav.down():nav.up();
        case '-': return invY?nav.up():nav.down();
        case '*': return nav.enter();
        case '/': return nav.esc();
        default: break;
      }
    }
    return false;
  }
};
