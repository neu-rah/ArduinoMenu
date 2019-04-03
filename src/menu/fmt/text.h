/* -*- C++ -*- */
#pragma once

namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void fmtMenu(bool io) {
      if (io) {
        O::raw("Menu------");
        O::raw(endl);
        O::fmtMenu(io);
      } else {
        O::fmtMenu(io);
        O::raw("Menu------");
        O::raw(endl);
      }
    }
    void fmtItem(bool io) {
      if (io) {
        O::fmtItem(io);
      } else {
        O::fmtItem(io);
        O::raw(endl);
      }
    }
    void fmtTitle(bool io) {
      if (io) {
        O::fmtTitle(io);
      } else {
        O::fmtTitle(io);
        O::raw(endl);
      }
    }
  };

};
