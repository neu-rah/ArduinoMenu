/* -*- C++ -*- */
#pragma once

#include "../arduino.h"

namespace AM5{namespace Arduino{namespace Avr {
  struct Platform {
    static constexpr char* Name="Avr";
    using Idx=uint8_t;
  };
}}};
