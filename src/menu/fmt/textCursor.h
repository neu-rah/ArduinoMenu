/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// output text cursor as either ">" or " "
// TODO: customize at type level (as title wrap)

namespace Menu {

  template<typename O>
  struct TextCursorFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    using O::O;
    template<typename P>
    inline void fmtCursor(PrintHead<P> p,bool io) {
      if (io) {
        // Serial<<"fmtCursor "<<p.printer.selected(p)<<endl;
        O::raw(p.printer.selected(p)?">":" ");
        O::fmtCursor(p,io);
      } else O::fmtCursor(p,io);
    }
  };
};//Menu
