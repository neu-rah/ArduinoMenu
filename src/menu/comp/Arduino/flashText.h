/* -*- C++ -*- */
#pragma once
/**
* @file flashText.h
* @brief menu item component, provides flash stored text
*/

#include <menu.h>

// namespace Menu {
  using ConstText=const char[];

  template<typename T,const T* text>
  struct FlashText {
    template<typename I=Empty<Nil>>
    struct Part:I {
      using I::I;
      // using I::size;
      template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
      inline static void print(Nav& nav,Out& out,PathRef ref=self) {
        out.template raw<const __FlashStringHelper *>(reinterpret_cast<const __FlashStringHelper *>(text[0]));
      }
    };
  };
// };
