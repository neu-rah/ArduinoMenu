/* -*- C++ -*- */
#pragma once
/**
* @file flashText.h
* @brief menu item component, provides flash stored text
*/

#include <menu.h>

// namespace Menu {
  template<typename T,T* text,typename O=Empty<>>
  struct FlashText:public O {
    template<typename Nav,typename Out>
    inline void print(Nav& nav,Out& out) {
      out.raw(reinterpret_cast<const __FlashStringHelper *>(text[0]));
    }
  };
// };
