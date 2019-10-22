/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu format, wrap the title between characters, default: '[' ']'
*/

#include <menu.h>

// namespace Menu {
  template<typename O,char open='[',char close=']'>
  struct TitleWrapFmt:public O {
    template<bool io,typename I,typename Out,typename Nav>
    static inline void fmtTitle(Nav& nav,Idx n=0) {
      if (io) {
        Out::raw(open);
        O::template fmtTitle<io,I,Out,Nav>(nav,n);
      } else {
        O::template fmtTitle<io,I,Out,Nav>(nav,n);
        Out::raw(close);
      }
    }
  };
// };
