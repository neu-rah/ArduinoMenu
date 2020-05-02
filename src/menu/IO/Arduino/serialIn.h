/* -*- C++ -*- */
#pragma once

#include "../../../staticMenu.h"

namespace Menu {

  template<typename Dev,Dev& dev>
  struct SerialIn {
    template<typename In=Quiet<Nil>>
    struct Part:In {
      template<typename Nav>
      inline bool parseKey(Nav& nav) {
        return dev.available()?
          In::parseCmd(nav,dev.read()):
          In::parseKey(nav);
      }
    };
  };
};
