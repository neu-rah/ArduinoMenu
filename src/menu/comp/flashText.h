/* -*- C++ -*- */
#pragma once

#include <menu.h>

template<typename T,T* text,typename O=Empty<>>
struct FlashText:public O {
  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    out.raw(reinterpret_cast<const __FlashStringHelper *>(text[0]));
  }
};
