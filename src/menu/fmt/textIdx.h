/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, print option index
*/

namespace Menu {
  template<typename O>
  struct TextIdxFmt:public O {
    template<bool io,bool toPrint=true>
    inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      if(io) {
        if (n<9) O::template print<int,This,toPrint>(n+1,*this,Roles::Index);
        else O::template print<char,This,toPrint>(' ',*this,Roles::Index);
      }
      O::template fmtIndex<io,toPrint>(n,s,e,m);
    }
  };
};
