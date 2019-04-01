/* -*- C++ -*- */
#pragma once

namespace Menu {

  template<typename O>
  struct TextFmt:public O {
    using RAW_DEVICE=typename O::RAW_DEVICE;//must have a raw device!
    void fmtTitle(bool io) {
      if (!io) O::raw(endl);
    }
  };

};
