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
        out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtIndex(NavHead& nav,OutHead& out) {
      if (io) {
        // NavHead::Printer::raw('[');
        if (idx<9) out.raw(idx+1);
        else out.raw(' ');
        out.raw(')');
        out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    inline void fmtItem(NavHead& nav,OutHead& out) {
      if (io) out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      else {
        out.template fmtItem<NavHead,OutHead,ItemHead,io,idx>(nav,out);
        out.nl();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtTitle(NavHead& nav,OutHead& out) {
      if (io) {
        out.raw('[');
        out.template fmtTitle<NavHead,OutHead,ItemHead,io,idx>(nav,out);
      } else {
        out.template fmtTitle<NavHead,OutHead,ItemHead,io,idx>(nav,out);
        out.raw(']');
        out.nl();
      }
    }
  };

};//AM5 namespace
