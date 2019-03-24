/* -*- C++ -*- */
#pragma once

#include <cstdint>
#include <string>
using namespace std;
#include "../menu.h"

namespace AM5{namespace Std {
  struct Framework {
    static constexpr char* Name="C/C++ std iostream";
    using RoleType=uint16_t;
  };
  struct Platform {
    static constexpr char* Name="Std IO";
    using Idx=size_t;
    using Str=string;
  };
}};
