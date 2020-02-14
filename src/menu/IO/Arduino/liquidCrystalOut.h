/* -*- C++ -*- */
#pragma once
/**
* @file liquidCrystalOut.h
* @author Rui Azevedo
* @brief use arduino standard LCD library as menu output
*/

#include <LiquidCrystal.h>
#include <menu.h>

namespace Menu {
  template<LiquidCrystal& dev>
  struct LiquidCrystalOut {
    template<typename O>
    struct Part:O {
      using O::O;
      using This = LiquidCrystalOut<dev>::Part<O>;
      template<bool toPrint=true>
      inline static void nl() {if(toPrint) dev.println();}
      static inline void setCursor(Idx x,Idx y) {
        trace(MDO<<"lcd.setCursor("<<x<<","<<y<<") "<<endl);
        dev.setCursor(x,y);
      }
      template<typename T,bool toPrint=true>
      inline void raw(T i) {
        trace(MDO<<"lcd.raw(\""<<i<<"\") "<<endl);
        dev.print(i);}
      inline void clrLine(Idx n) {
        trace(MDO<<"lcd.clrLine("<<n<<") "<<endl);
        dev.setCursor(0,n);
        for(int n=0;n<O::obj().width();n++) dev.print(' ');
      }
    };
  };
};
