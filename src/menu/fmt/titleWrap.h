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
    template<bool io,typename Out,typename Nav>
    static inline void fmtTitle(Nav& nav,Out& out,Idx n=0) {
      if (io) {
        out.raw(open);
        O::template fmtTitle<io,Out,Nav>(nav,out,n);
      } else {
        O::template fmtTitle<io,Out,Nav>(nav,out,n);
        out.raw(close);
      }
    }
  };
// };
