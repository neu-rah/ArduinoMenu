/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Dev,Dev& dev,typename O=Void<>>
struct OutStream:O {
  template<typename T> inline static void raw(T o) {dev<<o;}
  inline static void nl() {dev<<endl;}
};

using Console=OutStream<decltype(cout),cout,TextMeasure>;

//top level printer
template<typename Out>
struct MenuOut:public Data<Out> {
  using This=MenuOut<Out>;
  static inline void setTop(idx_t n) {Data<Out>::data.setTop(n);}
  static inline idx_t top() {return This::data.top();}
  template<typename I,typename Nav=Drift<>>
  inline static void printMenu(const I& i,const Nav& nav) {
    Out::template printMenu<I,This,Nav>(i,nav,reinterpret_cast<This&>(This::data));
  }
};
