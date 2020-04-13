/* -*- C++ -*- */
#pragma once

#include "../../../staticMenu.h"

namespace Menu {

  template<typename Dev,Dev& dev>
  struct SerialIn {
    template<typename In=Quiet<Nil>>
    struct Part:In {
      template<typename Nav>
      static bool cmd(Nav& nav) {
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
