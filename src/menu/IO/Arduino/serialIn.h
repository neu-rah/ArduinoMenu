/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev,typename O=Void<Nil>>
struct SerialIn:O {
  template<typename Nav>
  inline static bool cmd(Nav& nav) {
    if (dev.available()) {
      char key=dev.read();
      switch(key) {
        case '+': return nav.template cmd<Cmd::Up>();
        case '-': return nav.template cmd<Cmd::Down>();
        case '*': return nav.template cmd<Cmd::Enter>();
        case '/': return nav.template cmd<Cmd::Esc>();
        default:break;
      }
    }
    return false;
  }
};
