/* -*- C++ -*- */
#pragma once

#include "../../../staticMenu.h"

namespace Menu {
  template<typename Dev,Dev& dev>
  struct SerialIn {
    template<typename In=Quiet<Nil>>
    struct Part:In {
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
  };
};
