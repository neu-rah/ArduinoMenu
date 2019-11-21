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
  using This=TextFmt<O>;
  template<bool io,bool toPrint=true>
  inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) out.nl();
    // O::raw("*--------*");
    // out.nl();
  }
  template<bool io,bool toPrint=true>
  inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw("----------");
    // O::nl();
  }
  template<bool io,bool toPrint=true>
  inline void fmtBody(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"| ":" |");
    // out.nl();
  }
  template<bool io,bool toPrint=true>
  inline void fmtName(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"«":"»");
  }
  template<bool io,bool toPrint=true>
  inline void fmtValue(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"{":"}");
  }
  template<bool io,bool toPrint=true>
  inline void fmtUnit(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // O::raw(io?"(":")");
  }
  template<bool io,bool toPrint=true>
  inline void fmtMode(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) O::raw("[");
    // if (io) _trace(MDO<<n<<s<<e<<(int)m);
    if(io) switch(m) {
      case Modes::Normal: O::template raw<char,This,toPrint>(' ',*this,Roles::Mode);break;
      case Modes::Edit: O::template raw<char,This,toPrint>(s?':':' ',*this,Roles::Mode);break;
      case Modes::Tune: O::template raw<char,This,toPrint>(s?'>':' ',*this,Roles::Mode);break;
    }
    // if (!io) O::raw("]");
  }
  template<bool io,bool toPrint=true>
  inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (!io) out.nl();
  }
  template<bool io,bool toPrint=true>
  inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (!io) O::template nl<This,toPrint>(*this);
  }
  template<bool io,bool toPrint=true>
  inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) {
      if (n<9) O::template raw<char,This,toPrint>(n+1,*this,Roles::Index);
      else O::template raw<char,This,toPrint>(' ',*this,Roles::Index);
    }
  }
  template<bool io,bool toPrint=true>
  inline void fmtCursor(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if (io) _trace(MDO<<n<<s<<e<<(int)m);
    if (io) O::template raw<char,This,toPrint>(s?((e?'>':'-')):' ',*this,Roles::Cursor);
  }
};
