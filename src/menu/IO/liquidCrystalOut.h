/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use arduino standard LCD library as menu output

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <menu.h>

template<typename Dev,Dev& dev, typename O=FullPrinter<>>
struct LiquidCrystalOut:public O {
  using O::O;
  using This = LiquidCrystalOut<Dev,dev,O>;
  template<typename T>
  inline void raw(T i) {
    // Serial<<"LCDOutDef::raw("<<i<<")"<<endl;
    // if (!O::operator bool()) return;//TODO: this is naive, we need to measure
    // if (O::posY()+scrlPosY()>O::height()) return;
    dev.setCursor(O::posX(),O::posY());
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
