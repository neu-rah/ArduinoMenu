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
  //Output Device Operation
  enum class OOP {RawOut,Measure};

  template<LiquidCrystal& dev, typename O=FullPrinter<>>
  struct LiquidCrystalOut:public O {
    using O::O;
    using This = LiquidCrystalOut<dev,O>;
    template<OOP op=OOP::RawOut>
    inline void nl() {O::useY();}
    template<typename T,OOP op=OOP::RawOut>
    inline void raw(T i) {
      if (This::posY()<0) return;
      if (This::posY()>This::height()) return;
      dev.setCursor(O::posX(),O::posY());
      trace(Serial<<"lcd.setCursor("<<O::posX()<<","<<O::posY()<<") "<<i<<endl);
      if (op==OOP::RawOut) O::useX(dev.print(i));
      else O::useX(O::measure(i));
    }
    inline void clear() {
      O::newView();
      dev.clear();
    }
    template<bool io,typename Nav,typename Out,typename I>
    static inline void fmtItem(Nav& nav,Out& out,I& i,idx_t n) {
      if (io) {
        if (out.posY()<0) return;//O::useX(O::measure(i));//we only need to measure lines!
        if (out.posY()>out.height()) return;
        dev.setCursor(0,out.posY());
        for(int n=0;n<out.width();n++) dev.print(" ");
      }
      O::template fmtItem<io,Nav,Out,I>(nav,out,i,n);
    }
  };
// };
