/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use arduino standard LCD library as menu output

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <streamFlow.h>
#include "../out.h"
#include "../nav.h"
#include "../panels.h"
#include "../fmt/text.h"
#include "../fmt/textCursor.h"
#include "../fmt/titleWrap.h"
#include "../printers.h"
// #include "../fmt/debug.h"

namespace Menu {

  template<typename P,typename Dev,Dev& dev, typename O=Void>
  struct LCDOutDef:public O {
    using RAW_DEVICE = LCDOutDef<P,Dev,dev,O>;
    using Parts=P;
    using O::O;
    // static inline void endl() {O::useY();}//the viewport will catch it
    template<typename T>
    inline void raw(T i) {
      // Serial<<"LCDOutDef::raw("<<i<<")"<<endl;
      // if (!O::operator bool()) return;//TODO: this is naive, we need to measure
      // if (O::posY()+scrlPosY()>O::height()) return;
      dev.setCursor(O::posX(),O::posY());
      // Serial<<"lcd.setCursor("<<posX()<<","<<posY()<<") "<<i<<endl;
      O::useX(dev.print(i));
    }
    template<typename H>
    inline void clear(PrintHead<H>) {dev.clear();}
    template<typename H>
    inline void clearLine(PrintHead<H> p) {
      int line=p.line;//O::posY();
      // Serial<<"LCDOutDef::clearLine "<<p.pos<<(O::scrlPosY()>=0?"+":"")<<O::scrlPosY()<<"="<<line<<endl;
      // Serial<<"height:"<<p.printer.height()<<endl;
      if (line<0||line>=p.printer.height()) return;
      // Serial<<"LCDOutDef::clearLine "<<line<<endl;
      dev.setCursor(0,line);
      for(int n=0;n<p.printer.width();n++)
        dev.print(" ");
    }
  };

  using LCDParts=DeviceParts<
    ItemPrinter,//emit format messages for accel, cursor amd item
    TitlePrinter//emit format messages for titles (fmtTitle)
  >;

  // template<typename O>
  // using TitleWrap=TitleWrapFmt<O>;

  // template<typename O>
  // using SingleLineMenu=SingleLineScrollCtrl<O,1>;

  template<template<typename> class N=NavNode>
  using LCDFmt = Menu::Chain<//wrap inner types
    // DebugFmt,//add debug info when enabled
    // TextCursorFmt,//signal selected option on text mode
    TextFmt,//normal text format
    TitleWrap,//wrap title in []
    TitlePrinter,
    SelItemPrinter,//we only have a free line
    // FullPrinter,//print inner then options
    N//flat navigation control (no sub menus)
  >;

};//Menu

template<
  LiquidCrystal& lcd,
  typename Panel=Menu::Viewport<Menu::StaticPanel<0,0,16,2>>,
  typename Parts=Menu::LCDParts
> using LCDOutDev=Menu::LCDOutDef<
  Parts,
  LiquidCrystal,
  lcd,
  Panel
>;
