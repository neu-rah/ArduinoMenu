/* -*- C++ -*- */
#pragma once

//POSIX PC Keyboard
#include "menu/in.h"
namespace Menu {
  //PC keyboard arrows to menu commands
  struct PCArrows {
    template<typename In>
    struct Part:In {
      static constexpr bool isParser=true;
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        if (e&&k==91) return false;//wait for ext code
        bool r;
        switch(k) {
          case 66: r=nav.template cmd<Cmd::Up>();break;
          case 65: r=nav.template cmd<Cmd::Down>();break;
          case 13: r=nav.template cmd<Cmd::Enter>();break;
          case 27: r=nav.template cmd<Cmd::Esc>();break;
          case 67: r=nav.template cmd<Cmd::Right>();break;
          case 68: r=nav.template cmd<Cmd::Left>();break;
          default: r=nav.template cmd<Cmd::KeyCode>(k);break;
        }
        return r||In::parseCmd(nav,k,e);
      }
    };
  };
};