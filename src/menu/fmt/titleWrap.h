/* -*- C++ -*- */
#pragma once
/**
* @file titleWrap.h
* @author Rui Azevedo
* @brief ArduinoMenu format, wrap the title between characters, default: '[]'
*/

#include <menu.h>

// namespace Menu {
  template<typename O,char open='[',char close=']'>
  struct TitleWrapFmt:public O {
    template<bool io,typename Nav,typename Out,typename I>
    static inline void fmtTitle(Nav& nav,Out& out,I& i,idx_t n) {
      if (io) {
        out.raw(open);
        O::template fmtTitle<io,Nav,Out,I>(nav,out,i,n);
      } else {
        O::template fmtTitle<io>(nav,out,i,n);
        out.raw(close);
      }
    }
  };
// };
