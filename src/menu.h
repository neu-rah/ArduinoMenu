/* -*- C++ -*- */
#pragma once

#include "menu/base.h"
#include "menu/out.h"
#include "menu/items.h"

namespace Menu {
  template<typename O>
  MenuOut& MenuOutCap<O>::operator<<(Item& i) {i.out(*this);return *this;}

  inline MenuOut& operator<<(MenuOut& o, Item& i) {i.out(o);return o;}
};//Menu
