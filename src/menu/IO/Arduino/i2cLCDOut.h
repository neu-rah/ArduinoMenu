/* -*- C++ -*- */
#pragma once
/**
* @file i2cLCDOut.h
* @author Rui Azevedo
* @brief use arduino I2C LCD with PCF8574
* @driver https://github.com/mathertel/LiquidCrystal_PCF8574
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <staticMenu.h>

namespace Menu {

  template<LiquidCrystal_PCF8574& dev>
  struct I2CLCDOut {
    template<typename O>
    struct Part:O {
      using O::O;
      using This = I2CLCDOut<dev>::Part<O>;
      template<bool toPrint=true>
      inline static void nl() {}
      static inline void setCursor(Idx x,Idx y) {
        trace(MDO<<"lcd.setCursor("<<x<<","<<y<<") "<<endl);
        dev.setCursor(x,y);
      }
      template<typename T,bool toPrint=true>
      inline void raw(T i) {
        trace(if(toPrint) MDO<<"lcd.raw(\""<<i<<"\") "<<endl);
        if(toPrint) {
          setCursor(O::obj().posX(),O::obj().posY());
          dev.print(i);
        }
      }
      template<bool toPrint=true>
      inline void clrLine(Idx n) {
        trace(if(toPrint) MDO<<"lcd.clrLine("<<n<<") "<<endl);
        dev.setCursor(0,O::orgY()+n);
        if(toPrint) {
          for(int n=0;n<O::obj().width();n++) dev.print(' ');
          dev.setCursor(0,O::orgY()+n);
        }
      }
    };
  };

};
