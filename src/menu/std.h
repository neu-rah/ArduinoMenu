/* -*- C++ -*- */
#pragma once

#include "../menu.h"

namespace AM5{namespace Std {
  struct Framework {
    static constexpr char* Name="C/C++ std iostream";
  };
  struct Platform {
    static constexpr char* Name="Std IO";
    using Idx=size_t;
  };
}};
