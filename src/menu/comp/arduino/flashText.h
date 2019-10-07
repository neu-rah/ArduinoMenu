/* -*- C++ -*- */
#pragma once
/**
* @file flashText.h
* @brief menu item component, provides flash stored text
*/

#include <menu.h>

// namespace Menu {
  using ConstText=const char[];

  template<typename T,T* text,typename I=Empty<>>
  struct FlashText:public I {
    using I::I;
    using I::size;
    template<typename Out> inline static void print() {
      Out::raw(reinterpret_cast<const __FlashStringHelper *>(text[0]));
    }
    template<typename It,typename Nav,typename Out,Roles P=Roles::Raw>
    inline static void print(It& it,Nav& nav) {print<Out>();}
  };
// };
