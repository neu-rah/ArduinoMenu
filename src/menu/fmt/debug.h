/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// output addicional debug information
// when enabled by -DMENU_DEBUG

// #include "../names.h"

namespace Menu {

  template<typename O>
  struct DebugFmt:public O {
    using O::O;
    static bool debugFmt;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P> void fmtMenu(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMenu|");
        O::fmtMenu(p,io);
      } else {
        O::fmtMenu(p,io);
        if (debugFmt) O::raw("|fmtMenu}");
      }
    }
    template<typename P> void fmtPanel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtPanel|");
        O::fmtPanel(p,io);
      } else {
        O::fmtPanel(p,io);
        if (debugFmt) O::raw("|fmtPanel}");
      }
    }
    template<typename P> void fmtTitle(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtTitle|");
        O::fmtTitle(p,io);
      } else {
        O::fmtTitle(p,io);
        if (debugFmt) O::raw("|fmtTitle}");
      }
    }
    template<typename P> void fmtItem(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtItem|");
        O::fmtItem(p,io);
      } else {
        O::fmtItem(p,io);
        if (debugFmt) O::raw("|fmtItem}");
      }
    }
    template<typename P> void fmtAccel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtAccel|");
        O::fmtAccel(p,io);
      } else {
        O::fmtAccel(p,io);
        if (debugFmt) O::raw("|fmtAccel}");
      }
    }
    template<typename P> void fmtCursor(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtCursor|");
        O::fmtCursor(p,io);
      } else {
        O::fmtCursor(p,io);
        if (debugFmt) O::raw("|fmtCursor}");
      }
    }
    template<typename P> void fmtLabel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtLabel|");
        O::fmtLabel(p,io);
      } else {
        O::fmtLabel(p,io);
        if (debugFmt) O::raw("|fmtLabel}");
      }
    }
    template<typename P> void fmtMode(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMode|");
        O::fmtMode(p,io);
      } else {
        O::fmtMode(p,io);
        if (debugFmt) O::raw("|fmtMode}");
      }
    }
    template<typename P> void fmtValue(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtValue|");
        O::fmtValue(p,io);
      } else {
        O::fmtValue(p,io);
        if (debugFmt) O::raw("|fmtValue}");
      }
    }
    template<typename P> void fmtUnit(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtUnit|");
        O::fmtUnit(p,io);
      } else {
        O::fmtUnit(p,io);
        if (debugFmt) O::raw("|fmtUnit}");
      }
    }
  };
};
