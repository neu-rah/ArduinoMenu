/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
#include "../menu.h"

namespace AM5{namespace Arduino {
  struct Framework {
    static constexpr char* Name="Arduino";
    using RoleType=uint16_t;
  };
}};
