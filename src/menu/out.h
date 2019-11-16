/* -*- C++ -*- */
#pragma once

#include "api.h"

template<typename Dev,Dev& dev,typename O=Void>
struct StreamOut:O {
  inline static void nl() {dev<<endl;}
  template<typename T>
  inline static void raw(T o) {dev<<o;}
  // constexpr static inline Idx freeY() {return idx_max;}
};
