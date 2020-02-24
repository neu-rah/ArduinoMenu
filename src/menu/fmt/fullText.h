/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu text format, add `\n` at title and item end, print index and text cursor
*/

namespace Menu {

  struct TextFmt {
    template<typename O>
    struct Part:O {
      using This=TextFmt::Part<O>;
      // template<bool io,bool toPrint=true>
      // inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      //   O::template fmtPanel<io,toPrint>(n,s,e,m);
      // }
      template<bool io,bool toPrint=true>
      inline void fmtMenu(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        O::template fmtMenu<io,toPrint>(n,s,e,m);
      }
      // template<bool io,bool toPrint=true>
      // inline void fmtBody(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      //   O::template fmtBody<io,toPrint>(n,s,e,m);
      // }
      template<bool io,bool toPrint=true>
      inline void fmtName(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        if (io)O::template fmtName<io,toPrint>(n,s,e,m);
      }
      // template<bool io,bool toPrint=true>
      // inline void fmtValue(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      //   O::template fmtValue<io,toPrint>(n,s,e,m);
      // }
      template<bool io,bool toPrint=true>
      inline void fmtUnit(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        O::template fmtUnit<io,toPrint>(n,s,e,m);
      }
      template<bool io,bool toPrint=true>
      inline void fmtMode(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        if(io) switch(m) {
          case Mode::Normal: O::template raw<char,toPrint>(' ');break;
          case Mode::Edit: O::template raw<char,toPrint>(s?':':' ');break;
          case Mode::Tune: O::template raw<char,toPrint>(s?'>':' ');break;
        }
    		O::template fmtMode<io,toPrint>(n,s,e,m);
      }
      // template<bool io,bool toPrint=true>
      // inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
      //   O::template fmtTitle<io,toPrint>(n,s,e,m);
      // }
      template<bool io,bool toPrint=true>
      inline void fmtItem(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
    		O::template fmtItem<io,toPrint>(n,s,e,m);
        if (!io) O::obj().template nl<toPrint>();
      }
      template<bool io,bool toPrint=true>
      inline void fmtIndex(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        if(io) {
          if (n<9) O::template raw<char,toPrint>('1'+n);
          else O::template raw<char,toPrint>(' ');
        }
        O::template fmtIndex<io,toPrint>(n,s,e,m);
      }
      template<bool io,bool toPrint=true>
      inline void fmtCursor(Idx n=0,bool s=false,bool e=true,Mode m=Mode::Normal) {
        if (io) O::template raw<char,toPrint>(s?((e?'>':'-')):' ');
        O::template fmtCursor<io,toPrint>(n,s,e,m);
      }
    };
  };
};
