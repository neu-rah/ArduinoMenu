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
  template<ostream& dev=cout,typename O=FullPrinter<>>
  struct Console:public RawOut<ostream&,dev,O> {
    static inline void nl() {dev<<endl;}
  };
// }
