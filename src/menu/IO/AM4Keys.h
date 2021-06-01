/* -*- C++ -*- */
#pragma once

#include "../sys/base.h"

namespace Menu {

  struct AM4Parser {
    template<typename In>
    struct Part:In {
      using Base=In;
      using This=Part<In>;
      using Base::Base;
      Part(const This&)=delete;
      Part& operator=(const This&)=delete;
      // static constexpr bool isParser=true;
      template<typename Nav>
      bool parseCmd(Nav& nav,Key k,bool e=false) {
        bool r;
        switch(k) {
          case '+': r=nav.template cmd<Cmd::Up>();break;
          case '-': r=nav.template cmd<Cmd::Down>();break;
          case '*': r=nav.template cmd<Cmd::Enter>();break;
          case '/': r=nav.template cmd<Cmd::Esc>();break;
          default:  r=nav.template cmd<Cmd::KeyCode>(k);break;
        }
        return r||In::parseCmd(nav,k,e);
      }
    };
  };

};
