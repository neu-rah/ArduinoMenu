/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use std console as menu output

#include <ostream>
using namespace std;

#include "../out.h"

namespace Menu {
  // using ConsoleOutDef=OutDev<ostream,cout,Void>;
  template<ostream& dev=cout, typename O=Void>
  struct ConsoleOutDef:public O {
    enum RAW_DEVICE {};
    // using MUST_BE_AT_OUTPUT_BASE=O::OUTPUT_BASE;//or maybe not
    static inline void raw(const char*i) {dev<<i;}
    static inline void raw(char i) {dev<<i;}
    static inline void raw(unsigned char i) {dev<<i;}
    static inline void endl() {dev<<std::endl;}
    //.. add more type here
  };
};//Menu
