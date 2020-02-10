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
  // template<bool io,bool toPrint=true>
  // inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtPanel<io,toPrint>(n,s,e,m);
  // }
  template<bool io,bool toPrint=true>
  inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    O::template fmtMenu<io,toPrint>(n,s,e,m);
  }
  // template<bool io,bool toPrint=true>
  // inline void fmtBody(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtBody<io,toPrint>(n,s,e,m);
  // }
  // template<bool io,bool toPrint=true>
  // inline void fmtName(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtName<io,toPrint>(n,s,e,m);
  // }
  // template<bool io,bool toPrint=true>
  // inline void fmtValue(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtValue<io,toPrint>(n,s,e,m);
  // }
  // template<bool io,bool toPrint=true>
  // inline void fmtUnit(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtUnit<io,toPrint>(n,s,e,m);
  // }
  template<bool io,bool toPrint=true>
  inline void fmtMode(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) switch(m) {
      case Modes::Normal: O::template raw<char,toPrint>(' ');break;
      case Modes::Edit: O::template raw<char,toPrint>(s?':':' ');break;
      case Modes::Tune: O::template raw<char,toPrint>(s?'>':' ');break;
    }
		O::template fmtMode<io,toPrint>(n,s,e,m);
  }
  // template<bool io,bool toPrint=true>
  // inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   O::template fmtTitle<io,toPrint>(n,s,e,m);
  // }
  template<bool io,bool toPrint=true>
  inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
		O::template fmtItem<io,toPrint>(n,s,e,m);
    if (!io) O::nl();
  }
  template<bool io,bool toPrint=true>
  inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if(io) {
      if (n<9) O::template raw<char,toPrint>('1'+n);
      else O::template raw<char,toPrint>(' ');
    }
    O::template fmtIndex<io,toPrint>(n,s,e,m);
  }
  template<bool io,bool toPrint=true>
  inline void fmtCursor(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (io) O::template raw<char,toPrint>(s?((e?'>':'-')):' ');
    O::template fmtCursor<io,toPrint>(n,s,e,m);
  }
};
