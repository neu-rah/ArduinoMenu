/* -*- C++ -*- */
#pragma once

#include "../../out.h"
using SerialOut=StreamOut<decltype(Serial),Serial>;

template<typename Dev=decltype(Serial), Dev& dev=Serial,typename O=Void<>>
struct ArduinoPrintOut:O {
  inline static void nl() {dev.println();}
  template<typename T> inline static void raw(T o) {dev.print(o);}
};

#ifdef MENU_DEBUG
  template<typename O>
  using Debug=DebugOut<ArduinoPrintOut<>,O> {};
#endif
