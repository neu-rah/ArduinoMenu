/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
#include <streamFlow.h>
#include "../out.h"

namespace Menu {

  template<typename P,decltype(Serial)& dev=Serial, typename O=Void>
  struct SerialOutDev:public O {
    using RAW_DEVICE = SerialOutDev<P,dev,O>;
    using Parts=P;
    // using MUST_BE_AT_OUTPUT_BASE=O::OUTPUT_BASE;//or maybe not
    static inline void raw(const char*i) {dev<<i;}
    static inline void raw(char i) {dev<<i;}
    static inline void raw(unsigned char i) {dev<<i;}
    static inline void raw(endlObj i) {dev<<i;}
    static inline void endl() {dev<<::endl;}
    static inline void raw(const __FlashStringHelper *i) {dev.print(i);}
    //.. add more type here
  };
};//Menu
