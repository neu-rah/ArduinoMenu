/* -*- C++ -*- */
#pragma once

#include "menu/base.h"
#include "menu/out.h"
#include "menu/items.h"
#include "menu/fmt/debug.h"

namespace Menu {
  template<typename O>
  MenuOut& MenuOutCap<O>::operator<<(Item& i) {i.out(*this);return *this;}

  inline MenuOut& operator<<(MenuOut& o, Item& i) {i.out(o);return o;}

  #ifdef MENU_DEBUG
    template<typename O>
    bool DebugFmt<O>::debugFmt=true;
  #else
    template<typename O>
    bool DebugFmt<O>::debugFmt=false;
  #endif

  #include "menu/out.hpp"
};//Menu
