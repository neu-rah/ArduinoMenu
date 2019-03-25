/* -*- C++ -*- */
#pragma once

#include "../menu.h"
#include "out.h"

namespace Menu {
  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------
  struct Item {
    virtual void out(MenuOut& o) {}
  };

  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    void out(MenuOut &o) override {O::out(o);}
  };

  //static composition blocks -----------------------
  struct Empty {
    static inline void out(MenuOut&) {}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline void out(MenuOut &o) {o<<text;}
  };
};//Menu
