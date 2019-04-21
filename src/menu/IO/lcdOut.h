/* -*- C++ -*- */
#pragma once

#include "../base.h"
#include <LiquidCrystal.h>

namespace AM5 {
  template<
    LiquidCrystal& device,
    typename O=Void<>
  > struct LCDOut:public O {
    using This=LCDOut<device,O>;
    template<typename Nav,typename T>
    static inline void raw(T o) {
      device.setCursor(Nav::posX(),Nav::posY());
      Nav::useX(device.print(o));
    }
  };
};//AM5 namespace
