/* -*- C++ -*- */
#pragma once

#include "../../../menu.h"

template<typename Dev,Dev& dev,typename O=TextMeasure>
struct SerialOut:O {
  template<typename Out,bool toPrint=true>
  inline static void nl(Out&) {dev.println();}
  template<typename T>
  inline static void raw(T o,Roles role=Roles::Raw) {
    dev.print(o);
  }
};
