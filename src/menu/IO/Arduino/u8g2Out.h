/* -*- C++ -*- */
#pragma once

template<int charWidth,int charHeight,typename O>
struct GfxOut:O {//O must be a panel
  inline static constexpr int maxCharWidth() {return charWidth;}
  inline static constexpr int maxCharHeight() {return charHeight;}
  int textWidth(const char*) const;
};

template<typename Dev,Dev& dev,int charWidth,int charHeight,typename O>
struct CursorlessGfxOut:GfxOut<Dev,dev,O> {
  int
};

/*
Implement this drawing styles:
- measure option to account for geometry (->clipping)
- use predetermined size for options -> clipping
- use liquid layout with option positioning -> clipping => cursor position

implement:
- CursorlessOut - store printing coordinates (cursor) for devices that do not store it
- Clipping <- this should be part of the device... however must be composed above
*/
struct U8g2Out:O {
  inline static void nl() {dev.println();}
  template<typename T>
  inline static void raw(T o) {dev.print(o);}
};
