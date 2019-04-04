/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// store menu data on flash memory (Arduino framework)

#include "../base.h"
#include <Arduino.h>

namespace Menu {
  template<size_t n,typename O>
  class FlashMenuDef:public O {
  protected:
  public:
    Item const* data[n] PROGMEM;
    template<typename... OO>
    inline FlashMenuDef(PGM_P title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline const Item& operator[](size_t i) const {return *data[i];}
  };

};//Menu
