/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu standard C++ stream output

#include <menu.h>
#include <iostream>

using namespace std;

// namespace Menu {
  template<ostream& dev=cout,typename O=TextMeasure>
  struct Console:public RawOut<ostream&,dev,O> {
    static inline void nl() {dev<<endl;}
    template<typename T>
    inline void raw(T i) {
      dev<<i;
    }
  };
// };
