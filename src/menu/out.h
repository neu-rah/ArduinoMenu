/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Dev,Dev& dev,typename O=Void<>>
struct OutStream:O {
  template<typename T> inline static void raw(T o) {dev<<o;}
  inline static void nl() {dev<<endl;}
};

using Console=OutStream<decltype(cout),cout,TextMeasure>;
