/* -*- C++ -*- */
#pragma once

#include "../base.h"
namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtCursor() {
      if (io) {
        NavHead::raw(NavHead::template selected<idx>()?(NavHead::template enabled<idx>()?'>':'-'):' ');
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtIndex() {
      if (io) {
        // NavHead::Printer::raw('[');
        if (idx<9) NavHead::raw(idx+1);
        else NavHead::raw(' ');
        NavHead::raw(')');
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtItem() {
      if (io) O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
      else {
        O::template fmtItem<NavHead,OutHead,ItemHead,io,idx>();
        OutHead::nl();
      }
    }
    template<typename NavHead,typename OutHead,typename ItemHead,bool io,idx_t idx>
    static inline void fmtTitle() {
      if (io) {
        NavHead::raw('[');
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>();
      } else {
        O::template fmtTitle<NavHead,OutHead,ItemHead,io,idx>();
        NavHead::raw(']');
        NavHead::nl();
      }
    }
  };

};//AM5 namespace
