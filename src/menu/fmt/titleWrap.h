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
    template<bool io,typename Out,typename Nav,typename I>
    static inline void fmtTitle(idx_t n,const Nav& nav) {
      if (io) {
        Out::raw(open);
        O::template fmtTitle<io,Out,Nav,I>(n,nav);
      } else {
        O::template fmtTitle<io,Out,Nav,I>(n,nav);
        Out::raw(close);
      }
    }
  };
// };
