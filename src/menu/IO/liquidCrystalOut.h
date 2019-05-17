/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief use arduino standard LCD library as menu output
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <menu.h>

// namespace Menu {
  template<LiquidCrystal& dev, typename O=TextMeasure>
  struct LiquidCrystalOut:public O {
    using O::O;
    using This = LiquidCrystalOut<dev,O>;
    static inline void setCursor(idx_t x,idx_t y) {
      trace(MDO<<"lcd.setCursor("<<x<<","<<y<<") "<<endl);
      dev.setCursor(x,y);
    }
    template<typename T>
    inline void raw(T i) {
      dev.print(i);
    }
    template<typename Out>
    static inline void clrLine(Out& out,idx_t n) {
      dev.setCursor(0,n);
      for(int n=0;n<out.width();n++) dev.print(" ");
    }
  };
// };
