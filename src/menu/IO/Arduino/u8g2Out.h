/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

// DO NOT USE - INCOMPLETE!

// template<int charWidth,int charHeight,typename O>
// struct GfxOut:O {//O must be a panel
//   inline static constexpr int maxCharWidth() {return charWidth;}
//   inline static constexpr int maxCharHeight() {return charHeight;}
//   int textWidth(const char*) const;
// };
//
// //use static size monometric font
// template<typename Dev,Dev& dev,int charWidth,int charHeight,typename O>
// struct MonoGfxOut:GfxOut<Dev,dev,O> {
//   int curX=0,curY=0;
//
// };
//
// /*
// Implement this drawing styles:
// - measure option to account for geometry (->clipping)
// - use predetermined size for options -> clipping
// - use liquid layout with option positioning -> clipping => cursor position
//
// implement:
// - CursorlessOut - store printing coordinates (cursor) for devices that do not store it
// - Clipping <- this should be part of the device... however must be composed above
// */

template<typename Dev,Dev& dev>
struct U8g2TextMeasure:public Void {
  template<typename T>
  static inline Idx measure(T o) {
    return String(o).length();
  }
  inline static constexpr int maxCharWidth() {return dev.getMaxCharWidth();}
  inline static constexpr int maxCharHeight() {return dev.getMaxCharHeight();}
  int textWidth(const char*s) const {return dev.getStrWidth(s);}
};

template<typename O>
struct U8g2Fmt:public O {
  // template<bool io>
  // inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
  //   //TODO can NOT position here!
  //   // O::setCursor(10,n*O::maxCharHeight()-O::top()*O::maxCharHeight()+O::ascent());
  // }
};

template<typename Dev,Dev& dev,typename O=Void>
struct U8g2Out:O,U8g2TextMeasure<Dev,dev> {
  // template<typename Out,bool toPrint=true>
  // inline static void nl(Out&) {}
  template<typename T>
  inline static void raw(T o,Roles role=Roles::Raw) {dev.print(o);}
  inline static int ascent() {return dev.getAscent()+1;}
  inline static void setCursor(int x,int y) {dev.setCursor(x,y);}
};
