/* -*- C++ -*- */
#pragma once
/**
* @file consoleOut.h
* @author Rui Azevedo
* @date Apr 2019
* @brief Use standard C++ stream as menu output
*/

#include <menu.h>
#include <iostream>

using namespace std;

template<ostream& dev=cout,typename O=TextMeasure>
struct Console:public RawOut<ostream&,dev,O> {
  static inline void nl() {dev<<'\r'<<endl;}
  template<typename T>
  inline void raw(T i) {
    dev<<i;
  }
};

template<typename O=Void<>>
using StdOut=Console<cout,O>;
