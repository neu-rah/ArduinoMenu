/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu format, wrap the between characters, default: '[]'
*/

#include <menu.h>

// namespace Menu {
  template<typename O=Void<TM>,char open='[',char close=']'>
  struct TitleWrap:public O {
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
