/* -*- C++ -*- */
#pragma once

#include <U8x8lib.h>
#include "../../../menu.h"

template<typename O>
struct U8x8Fmt:public O {
  inline void fillLines(int n,int h=1) {
    uint8_t x=O::u8x8_dev_v.tx;
    uint8_t y=O::u8x8_dev_v.ty;
    while(h--) {
      int c=n;
      while(c--) O::raw(' ');
      O::u8x8_dev_v.setCursor(x,y+h);
    }
  }
  template<bool io>
  inline void fmtPanel(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    // if(io) O::u8x8_dev_v.clear();
    // O::template fmtPanel<io>(n,s,e,m);
  }
  template<bool io>
  inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    O::u8x8_dev_v.setInverseFont(io);
    if(io) fillLines(O::u8x8_dev_v.getCols());
    O::template fmtTitle<io>(n,s,e,m);
  }
  template<bool io>
  inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (io) {
      if (s) O::u8x8_dev_v.setInverseFont(true);
      fillLines(O::u8x8_dev_v.getCols());
    }
    O::template fmtItem<io>(n,s,e,m);
    if (s&&!io) O::u8x8_dev_v.setInverseFont(false);
  }
};

template<typename O>
struct BigTitleU8x8Out:O {
  template<bool io,bool toPrint=true>
  inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (toPrint) {
      O::u8x8_dev_v.setInverseFont(io);
      if(io) O::fillLines(O::u8x8_dev_v.getCols(),2);
      O::template fmtTitle<io>(n,s,e,m);
    }
  }
  using O::raw;
  template<typename T>
  inline void raw(T o,Roles role=Roles::Raw) {
    if (role==Roles::Title) {
      O::u8x8_dev_v.draw2x2String(O::u8x8_dev_v.tx,O::u8x8_dev_v.ty,(const char*)o);
      O::useY();
    } else O::raw(o,role);
  }
};

template<typename Dev,Dev& dev,typename O=TextMeasure>
struct U8x8Out:O {
  static constexpr Dev& u8x8_dev_v=dev;
  inline static void nl() {}

  template<typename T>
  inline static void raw(T o,Roles role=Roles::Raw) {dev.print(o);}

  inline static constexpr int ascent() {return 0;}
  inline static void setCursor(int x,int y) {dev.setCursor(x,y);}
};
