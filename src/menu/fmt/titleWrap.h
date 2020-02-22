/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu format, wrap the title between characters, default: '[' ']'
*/

#include <staticMenu.h>

namespace Menu {
  template<char open='[',char close=']'>
  struct TitleWrapFmt {
    template<typename O>
    struct Part:O {
      using This=TitleWrapFmt<open,close>::Part<O>;
      template<bool io,bool toPrint=true>
      inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        if (io) {
          O::template raw<char,toPrint>(open);
          O::template fmtTitle<io,toPrint>(n,s,e,m);
        } else {
          O::template fmtTitle<io,toPrint>(n,s,e,m);
          O::template raw<char,toPrint>(close);
        }
      }
    };
  };
};
