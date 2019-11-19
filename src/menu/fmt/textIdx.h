/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, print option index
*/

template<typename O>
struct TextIdxFmt:public O {
  template<bool io>
  inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) {
      if (n<9) O::raw(n+1);
      else O::raw(' ');
    }
  }
};
