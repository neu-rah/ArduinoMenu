/* -*- C++ -*- */
#pragma once

#include "../base.h"
namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtCursor(OutHead& out) {
      if (io) {
        OutHead::raw(NavHead::template selected<idx>()?(NavHead::template enabled<idx>()?'>':'-'):' ');
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtIndex(OutHead& out) {
      if (io) {
        // NavHead::Printer::raw('[');
        if (idx<9) OutHead::raw(idx+1);
        else OutHead::raw(' ');
        OutHead::raw(')');
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    inline void fmtItem(OutHead& out) {
      if (io) O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
        out.nl();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtTitle(OutHead& out) {
      if (io) {
        out.raw('[');
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>();
        out.raw(']');
        out.nl();
      }
    }
  };

};//AM5 namespace
