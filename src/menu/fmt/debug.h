/* -*- C++ -*- */
#pragma once

// #include "../names.h"

namespace Menu {

  template<typename O>
  struct DebugFmt:public O {
    static bool debugFmt;
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void fmtMenu(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMenu|");
        O::fmtMenu(io);
      } else {
        O::fmtMenu(io);
        if (debugFmt) O::raw("|fmtMenu}");
      }
    }
    void fmtPanel(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtPanel|");
        O::fmtPanel(io);
      } else {
        O::fmtPanel(io);
        if (debugFmt) O::raw("|fmtPanel}");
      }
    }
    void fmtTitle(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtTitle|");
        O::fmtTitle(io);
      } else {
        O::fmtTitle(io);
        if (debugFmt) O::raw("|fmtTitle}");
      }
    }
    void fmtItem(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtItem|");
        O::fmtItem(io);
      } else {
        O::fmtItem(io);
        if (debugFmt) O::raw("|fmtItem}");
      }
    }
    void fmtAccel(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtAccel|");
        O::fmtAccel(io);
      } else {
        O::fmtAccel(io);
        if (debugFmt) O::raw("|fmtAccel}");
      }
    }
    void fmtCursor(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtCursor|");
        O::fmtCursor(io);
      } else {
        O::fmtCursor(io);
        if (debugFmt) O::raw("|fmtCursor}");
      }
    }
    void fmtLabel(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtLabel|");
        O::fmtLabel(io);
      } else {
        O::fmtLabel(io);
        if (debugFmt) O::raw("|fmtLabel}");
      }
    }
    void fmtMode(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtMode|");
        O::fmtMode(io);
      } else {
        O::fmtMode(io);
        if (debugFmt) O::raw("|fmtMode}");
      }
    }
    void fmtValue(bool io) {
      if (io) {
        if (debugFmt) O::raw("{fmtValue|");
        O::fmtValue(io);
      } else {
        O::fmtValue(io);
        if (debugFmt) O::raw("|fmtValue}");
      }
    }
    void fmtUnit(bool io) {
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
