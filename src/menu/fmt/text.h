/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// output text format

namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    using O::O;
    template<typename P>
    inline void fmtMenu(PrintHead<P> p,bool io) {
      if (io) {
        O::raw("Menu------");
        O::endl();
        O::fmtMenu(p,io);
      } else {
        O::fmtMenu(p,io);
        O::raw("Menu------");
        O::endl();
      }
    }
    template<typename P>
    inline void fmtAccel(PrintHead<P> p,bool io) {
      if (io) {
        O::raw("[");
        if (p.pos<10) O::raw((int)p.pos);
        else O::raw(" ");
        O::raw("]");
        O::fmtAccel(p,io);
      } else {
        O::fmtAccel(p,io);
      }
    }
    template<typename P>
    inline void fmtCursor(PrintHead<P> p,bool io) {
      if (io) {
        O::raw(p.printer.selected(p)?">":" ");
        O::fmtCursor(p,io);
      } else {
        O::fmtCursor(p,io);
      }
    }
    template<typename P>
    inline void fmtItem(PrintHead<P> p,bool io) {
      if (io) {
        O::fmtItem(p,io);
      } else {
        O::fmtItem(p,io);
        O::endl();
      }
    }
    template<typename P>
    inline void fmtTitle(PrintHead<P> p,bool io) {
      if (io) {
        O::fmtTitle(p,io);
      } else {
        O::fmtTitle(p,io);
        O::endl();
      }
    }
  };

};
