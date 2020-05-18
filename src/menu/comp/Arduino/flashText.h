/* -*- C++ -*- */
#pragma once
/**
* @file flashText.h
* @brief menu item component, provides flash stored text
*/

#include <staticMenu.h>

namespace Menu {
  using ConstText=const char[];

  template<typename T,const T* text>
  struct FlashText {
    template<typename I=Empty<Nil>>
    struct Part:I {
      using I::I;
      template<typename Nav,typename Out,Op op=Op::Printing,bool delegate=true>
      inline void print(Nav& nav,Out& out,Idx level,bool selected) {
        out.template raw<const __FlashStringHelper *,op==Op::Printing>(reinterpret_cast<const __FlashStringHelper *>(text[0]));
        if (delegate) I::template print<Nav,Out,op>(nav,out,level,selected);
      }
    };
  };
};
