#pragma once

#include "menu/sys/base.h"
namespace Menu {
  template<typename O=Nil> 
  struct Framework:O {
    inline static constexpr uint32_t ms() {return 0;}
    inline static constexpr uint32_t us() {return ms()*1000;}
  };
};