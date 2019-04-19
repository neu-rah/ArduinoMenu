/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use arduino standard LCD library as menu output

#include "base.h"
#include <LiquidCrystal.h>

namespace AM5 {

  template</*typename P,*/typename Dev,Dev& dev, typename O=Void>
  struct LCDOutDef:public O {
    // using RAW_DEVICE = LCDOutDef<P,Dev,dev,O>;
    // using Parts=P;
    // using O::O;
    // static inline void endl() {O::useY();}//the viewport will catch it
    template<typename T>
    inline void raw(T i) {
      // Serial<<"LCDOutDef::raw("<<i<<")"<<endl;
      // if (!O::operator bool()) return;//TODO: this is naive, we need to measure
      // if (O::posY()+scrlPosY()>O::height()) return;
      dev.setCursor(O::posX(),O::posY());//TODO separate navigation function
      // Serial<<"lcd.setCursor("<<posX()<<","<<posY()<<") "<<i<<endl;
      O::useX(dev.print(i));
    }
    // template<typename H>
    // inline void clear(PrintHead<H>) {dev.clear();}
    // template<typename H>
    // inline void clearLine(PrintHead<H> p) {
    //   int line=p.line;//O::posY();
    //   // Serial<<"LCDOutDef::clearLine "<<p.pos<<(O::scrlPosY()>=0?"+":"")<<O::scrlPosY()<<"="<<line<<endl;
    //   // Serial<<"height:"<<p.printer.height()<<endl;
    //   if (line<0||line>=p.printer.height()) return;
    //   // Serial<<"LCDOutDef::clearLine "<<line<<endl;
    //   dev.setCursor(0,line);
    //   for(int n=0;n<p.printer.width();n++)
    //     dev.print(" ");
    // }
  };

  template<LiquidCrystal& lcd,typename O=Empty>
  using LCDOut=LCDOutDef<LiquidCrystal,lcd,O>;

};//AM5
