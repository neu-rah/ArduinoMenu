/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  template<typename O>
  struct TextFmt:public O {
    template<typename H,bool io>
    inline void fmtCursor(H& p) {
      if (io) {
        H::Printer::raw(p.selected()?(p.enabled()?'>':'-'):' ');
        O::template fmtItem<H,io>(p);
      } else {
        O::template fmtItem<H,io>(p);
      }
    }
    template<typename H,bool io>
    inline void fmtIndex(H& p) {
      if (io) {
        // H::Printer::raw('[');
        if (p.pos()<9) H::Printer::raw(p.pos()+1);
        else H::Printer::raw(' ');
        H::Printer::raw(')');
        O::template fmtItem<H,io>(p);
      } else {
        O::template fmtItem<H,io>(p);
      }
    }
    template<typename H,bool io>
    inline void fmtItem(H& p) {
      if (io) O::template fmtItem<H,io>(p);
      else {
        O::template fmtItem<H,io>(p);
        H::Printer::nl();
      }
    }
    template<typename H,bool io>
    inline void fmtTitle(H& p) {
      if (io) {
        H::Printer::raw('[');
        O::template fmtTitle<H,io>(p);
      } else {
        O::template fmtTitle<H,io>(p);
        H::Printer::raw(']');
        H::Printer::nl();
      }
    }
  };

};
