/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, add `\n` at title and item end, print index and text cursor
*/

template<typename O>
struct TextFmt:public O {
  template<bool io>
  inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) out.nl();
    // O::raw("*--------*");
    // out.nl();
  }
  template<bool io>
  inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw("----------");
    // O::nl();
  }
  template<bool io>
  inline void fmtBody(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"| ":" |");
    // out.nl();
  }
  template<bool io>
  inline void fmtName(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"«":"»");
  }
  template<bool io>
  inline void fmtValue(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"{":"}");
  }
  template<bool io>
  inline void fmtUnit(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"(":")");
  }
  template<bool io>
  inline void fmtMode(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) O::raw("[");
    // if (io) _trace(MDO<<n<<s<<e<<(int)m);
    if(io) switch(m) {
      case Modes::Normal: O::raw(' ');break;
      case Modes::Edit: O::raw(s?':':' ');break;
      case Modes::Tune: O::raw(s?'>':' ');break;
    }
    // if (!io) O::raw("]");
  }
  template<bool io>
  inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (!io) out.nl();
  }
  template<bool io>
  inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (!io) O::nl();
  }
  template<bool io>
  inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) {
      if (n<9) O::raw(n+1);
      else O::raw(' ');
    }
  }
  template<bool io>
  inline void fmtCursor(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) _trace(MDO<<n<<s<<e<<(int)m);
    if (io) O::raw(s?((e?'>':'-')):' ');
  }
};
