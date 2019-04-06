/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// output for devices that can position a cursor

namespace Menu {

  template<typename O>
  struct CursorPosFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    using O::O;

    template<typename P>
    inline void fmtItem(PrintHead<P> p,bool io) {
      if (io) O::fmtItem(p,io);
      else {
        // O::fmtItem(p,io);
        O::endl();
      }
    }

    template<typename P>
    inline void fmtTitle(PrintHead<P> p,bool io) {
      if (io) O::fmtTitle(p,io);
      else {
        // O::fmtTitle(p,io);
        O::endl();
      }
    }
  };

};
