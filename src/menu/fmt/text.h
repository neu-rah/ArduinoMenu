/* -*- C++ -*- */
#pragma once

#include "../base.h"
namespace AM5 {

  template<typename O>
  struct TextFmt:public O {
    template<typename H,typename I,bool io,size_t idx>
    inline void fmtCursor() {
      if (io) {
        H::raw(H::template selected<idx>()?(H::template enabled<idx>()?'>':'-'):' ');
        O::template fmtItem<H,I,io,idx>();
      } else {
        O::template fmtItem<H,I,io,idx>();
      }
    }
    template<typename H,typename I,bool io,size_t idx>
    inline void fmtIndex() {
      if (io) {
        // H::Printer::raw('[');
        if (idx<9) H::raw(idx+1);
        else H::raw(' ');
        H::raw(')');
        O::template fmtItem<H,I,io,idx>();
      } else {
        O::template fmtItem<H,I,io,idx>();
      }
    }
    template<typename H,typename I,bool io,size_t idx>
    inline void fmtItem() {
      if (io) O::template fmtItem<H,I,io,idx>();
      else {
        O::template fmtItem<H,I,io,idx>();
        H::nl();
      }
    }
    template<typename H,typename I,bool io,size_t idx>
    inline void fmtTitle() {
      if (io) {
        H::raw('[');
        O::template fmtTitle<H,I,io,idx>();
      } else {
        O::template fmtTitle<H,I,io,idx>();
        H::raw(']');
        H::nl();
      }
    }
  };

};//AM5 namespace
