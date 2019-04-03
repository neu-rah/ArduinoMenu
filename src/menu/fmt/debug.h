/* -*- C++ -*- */
#pragma once

// #include "../names.h"

namespace Menu {

  template<typename O>
  struct DebugFmt:public O {
    static bool debugFmt;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    template<typename P> void fmtMenu(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMenu|");
        O::fmtMenu(io);
      } else {
        O::fmtMenu(io);
        if (debugFmt) O::raw("|fmtMenu}");
      }
    }
    template<typename P> void fmtPanel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtPanel|");
        O::fmtPanel(io);
      } else {
        O::fmtPanel(io);
        if (debugFmt) O::raw("|fmtPanel}");
      }
    }
    template<typename P> void fmtTitle(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtTitle|");
        O::fmtTitle(io);
      } else {
        O::fmtTitle(io);
        if (debugFmt) O::raw("|fmtTitle}");
      }
    }
    template<typename P> void fmtItem(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtItem|");
        O::fmtItem(io);
      } else {
        O::fmtItem(io);
        if (debugFmt) O::raw("|fmtItem}");
      }
    }
    template<typename P> void fmtAccel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtAccel|");
        O::fmtAccel(io);
      } else {
        O::fmtAccel(io);
        if (debugFmt) O::raw("|fmtAccel}");
      }
    }
    template<typename P> void fmtCursor(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtCursor|");
        O::fmtCursor(io);
      } else {
        O::fmtCursor(io);
        if (debugFmt) O::raw("|fmtCursor}");
      }
    }
    template<typename P> void fmtLabel(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtLabel|");
        O::fmtLabel(io);
      } else {
        O::fmtLabel(io);
        if (debugFmt) O::raw("|fmtLabel}");
      }
    }
    template<typename P> void fmtMode(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMode|");
        O::fmtMode(io);
      } else {
        O::fmtMode(io);
        if (debugFmt) O::raw("|fmtMode}");
      }
    }
    template<typename P> void fmtValue(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtValue|");
        O::fmtValue(io);
      } else {
        O::fmtValue(io);
        if (debugFmt) O::raw("|fmtValue}");
      }
    }
    template<typename P> void fmtUnit(PrintHead<P> p,bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtUnit|");
        O::fmtUnit(io);
      } else {
        O::fmtUnit(io);
        if (debugFmt) O::raw("|fmtUnit}");
      }
    }
  };
};
