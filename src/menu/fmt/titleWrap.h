/* -*- C++ -*- */
#pragma once
/**
* @file titleWrap.h
* @author Rui Azevedo
* @brief ArduinoMenu format, wrap the title between characters, default: '[' ']'
*/

#include <menu.h>

// namespace Menu {
  template<typename O,char open='[',char close=']'>
  struct TitleWrapFmt:public O {
    template<bool io,typename Nav,typename Out,typename I>
    static inline void fmtTitle(idx_t n) {
      if (io) {
        Out::raw(open);
        O::template fmtTitle<io,Nav,Out,I>(n);
      } else {
        O::template fmtTitle<io,Nav,Out,I>(n);
        Out::raw(close);
      }
    }
  };
// };
