/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
#include "../out.h"

namespace Menu {
  template<decltype(Serial)& dev=Serial>
  using SerialOutDev=OutDev<decltype(Serial),dev,Void>;
};//Menu
