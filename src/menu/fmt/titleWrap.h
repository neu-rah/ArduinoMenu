/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// output text format

namespace Menu {

  template<typename O,char start='[',char end=']'>
  struct TitleWrapFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    using O::O;
    template<typename P>
    inline void fmtTitle(PrintHead<P> p,bool io) {
      if (io) {
        O::raw(start);
        O::fmtTitle(p,io);
      } else {
        O::fmtTitle(p,io);
        O::raw(end);
      }
    }
  };

};
