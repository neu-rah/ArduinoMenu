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
  using This=TextEditModeFmt<O>;
  template<bool io,bool toPrint=true>
  inline void fmtMode(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) switch(m) {
      case Modes::Normal: O::template print<char,This,toPrint>(' ',*this,Roles::Mode);break;
      case Modes::Edit: O::template print<char,This,toPrint>(s?':':' ',*this,Roles::Mode);break;
      case Modes::Tune: O::template print<char,This,toPrint>(s?'>':' ',*this,Roles::Mode);break;
    }
  }
};
