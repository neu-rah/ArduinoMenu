/* -*- C++ -*- */
#pragma once

#include "../items.h"

namespace Menu {

  template<typename O>
  class NavNode:public O {
    protected:
      size_t sel;
    public:
      constexpr static inline bool canNav() {return true;}
      void up() {if (sel>0) sel--;}
      void down() {if (sel<size()-1) sel++;}
      void left() {up();}
      void left() {down();}
      void enter() {}
      void esc() {}
  };

};//Menu namespace
