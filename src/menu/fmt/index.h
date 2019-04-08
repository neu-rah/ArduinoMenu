/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// print current item index

// #include "../names.h"

namespace Menu {

  template<typename O>
  struct IndexFmt:public O {
    using O::O;
    static bool debugFmt;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P>
    inline void fmtAccel(PrintHead<P> p,bool io) {
      if (io) {
        O::raw("[");
        if (p.pos<9) O::raw((int)p.pos+1);
        else O::raw(" ");
        O::raw("]");
        O::fmtAccel(p,io);
      } else {
        O::fmtAccel(p,io);
      }
    }
  };
};
