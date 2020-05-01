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

  // struct IndexAccel {
  //   template<typename In>
  //   struct Part:In {
  //     template<typename Nav>
  //     ActRes parseCmd(Nav& nav,int k) {
  //       switch(k) {
  //         case 0: return nav.template cmd<Cmd::Esc,0>();
  //         case 1: return nav.template <Cmd::Enter,1>();
  //       }
  //     }
  //   };
  // };

  // struct NumIds {
  //   template<typename In=None>
  //   struct Part:In {
  //     template<typename Nav>
  //     ActRes parseCmd(Nav& nav,int k) {
  //       static Idx id=0;
  //       if(isdigit(k)) {
  //         id*=10;
  //         id+=k-'0';
  //       } else switch(id) {
  //         case 0:
  //           if(nav.template activate<0>()==
  //
  //         // if (nav.template activate<id>()==...)
  //       };
  //       return In::cmd(nav,k);
  //     }
  //   };
  // };

};
