/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, print edit mode cursor for fields
*/

template<typename O>
struct TextEditModeFmt:public O {
  template<bool io,bool toPrint=true>
  inline void fmtMode(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) switch(m) {
      case Modes::Normal: O::raw(' ');break;
      case Modes::Edit: O::raw(s?':':' ');break;
      case Modes::Tune: O::raw(s?'>':' ');break;
    }
  }
};
