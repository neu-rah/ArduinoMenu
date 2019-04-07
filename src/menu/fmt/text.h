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
    inline void fmtItem(PrintHead<P> p,bool io) {
      if (io) {
        O::fmtItem(p,io);
        clearLine(p);
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
