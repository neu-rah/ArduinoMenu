/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
#include "../out.h"

namespace Menu {

  template<decltype(Serial)& dev=Serial, typename O=Void>
  struct SerialOutDev:public O {
    // inline void out(Item& i) override {i.out()}
    static inline void raw(const char*i) {dev<<i;}
    static inline void raw(char i) {dev<<i;}
    static inline void raw(unsigned char i) {dev<<i;}
    static inline void raw(const __FlashStringHelper *i) {dev.print(i);}
    //.. add more type here
  };
};//Menu
