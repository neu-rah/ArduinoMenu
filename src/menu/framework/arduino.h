#pragma once

#include "frameworks.h"
namespace Menu {

  template<typename O=Nil> 
  struct ArduinoFramework:Framework<O> {
    inline static uint32_t ms() {return millis();}
    inline static uint32_t us() {return micros();}
  };
  template<typename O=Nil> 
  struct TinyPlatform:ArduinoFramework<O> {
    using Idx=byte;
  };  
  template<typename O=Nil> 
  struct DefaultPlatform:ArduinoFramework<O> {
    using Idx=size_t;
  };  

};