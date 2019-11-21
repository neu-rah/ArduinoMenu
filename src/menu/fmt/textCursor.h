/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, print text cursor, either ">- "
*/

template<typename O>
struct TextCursorFmt:public O {
  template<bool io,bool toPrint=true>
  inline void fmtCursor(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) _trace(MDO<<n<<s<<e<<(int)m);
    if (io) O::raw(s?((e?'>':'-')):' ');
  }
};
