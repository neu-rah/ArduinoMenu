/* -*- C++ -*- */
#pragma once

#include "../base.h"
namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    inline void fmtCursor(NavHead& nav,OutHead& out) {
      if (io) {
        out.raw(nav.selected(idx)?(nav.enabled(idx)?'>':'-'):' ');
        O::template fmtCursor<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        O::template fmtCursor<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtIndex(NavHead& nav,OutHead& out) {
      if (io) {
        // NavHead::Printer::raw('[');
        if (idx<9) out.raw(idx+1);
        else out.raw(' ');
        out.raw(')');
        O::template fmtIndex<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        O::template fmtIndex<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    inline void fmtItem(NavHead& nav,OutHead& out) {
      if (io) O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
        out.nl();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtTitle(NavHead& nav,OutHead& out) {
      if (io) {
        out.raw('[');
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>(nav,out);
        out.raw(']');
        out.nl();
      }
    }
  };

};//AM5 namespace
