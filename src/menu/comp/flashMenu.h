/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// store menu data on flash memory (Arduino framework)

#include "../base.h"
#include <Arduino.h>

namespace Menu {
  template<Item* const* data,size_t n,typename O>
  class FlashMenuDef:public O {
  protected:
  public:
    using O::O;
    static inline size_t size() {return n;}
    inline Item& operator[](size_t i) {
      //strange pointer arithmetic!
      return *(Item*)pgm_read_ptr(data+i);
    }
  };

  using FlashData=PROGMEM Item* const;

};//Menu
