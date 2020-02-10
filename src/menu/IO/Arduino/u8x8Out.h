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
  template<bool io,bool toPrint=true>
  inline void fmtTitle(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    O::u8x8_dev_v.setInverseFont(io);
    if(io&&toPrint) fillLines(O::u8x8_dev_v.getCols());
    // O::template fmtTitle<io,toPrint>(n,s,e,m);//don't call inner!
  }
  template<bool io,bool toPrint=true>
  inline void fmtItem(Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (io) {
      if (s) O::u8x8_dev_v.setInverseFont(true);
      if (toPrint) fillLines(O::u8x8_dev_v.getCols());
    }
    O::template fmtItem<io,toPrint>(n,s,e,m);
    if (s&&!io) O::u8x8_dev_v.setInverseFont(false);
  }
};

template<typename O>
struct BigTitleU8x8Out:O {
  template<typename T,typename Out,bool toPrint=true>
  inline void print(T o,Out& out,Roles role=Roles::Raw) {
    if (role==Roles::Title) {
      O::u8x8_dev_v.setInverseFont(true);
      if (toPrint) O::u8x8_dev_v.draw2x2String(O::u8x8_dev_v.tx,O::u8x8_dev_v.ty,(const char*)o);
      O::u8x8_dev_v.setInverseFont(false);
      O::useY();
    } else O::template print<T,Out,toPrint>(o,out,role);
  }
};

template<typename Dev,Dev& dev,typename O=TextMeasure>
struct U8x8Out:O {
  static constexpr Dev& u8x8_dev_v=dev;
  template<bool toPrint=true>
  inline static void nl() {}

  template<typename T,bool toPrint=true>
  inline static void raw(T o,Roles role=Roles::Raw) {dev.print(o);}

  inline static constexpr int ascent() {return 0;}
  inline static void setCursor(int x,int y) {dev.setCursor(x,y);}
};
