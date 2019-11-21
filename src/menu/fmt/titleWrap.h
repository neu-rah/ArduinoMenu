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
    using This=TitleWrapFmt<O,open,close>;
    template<bool io,bool toPrint=true>
    inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
      if (io) {
        O::template raw<char,This,toPrint>(open,*this,Roles::Title);
        O::template fmtTitle<io>(n,s,e,m);
      } else {
        O::template fmtTitle<io>(n,s,e,m);
        O::template raw<char,This,toPrint>(close,*this,Roles::Title);
      }
    }
  };
// };
