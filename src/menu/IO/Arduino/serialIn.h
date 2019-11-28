/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev,typename O=Void>
struct SerialIn:O {
  template<bool onField=false>
  inline static Cmds cmd() {
    if (dev.available()) {
      char key=dev.read();
      switch(key) {
        case '+': return Cmds::Up;
        case '-': return Cmds::Down;
        case '*': return Cmds::Enter;
        case '/': return Cmds::Esc;
        default:break;
      }
    }
    return Cmds::None;
  }
};
