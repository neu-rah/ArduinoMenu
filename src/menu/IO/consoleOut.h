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
template<typename P,ostream& dev=cout, typename O=Void>
  struct ConsoleOutDef:public O {
    using RAW_DEVICE = ConsoleOutDef<P,dev,O>;
    using Parts=P;
    // using MUST_BE_AT_OUTPUT_BASE=O::OUTPUT_BASE;//or maybe not
    template<typename T>
    inline void raw(T i) {dev<<i;}
    // static inline void raw(const char*i) {dev<<i;}
    // static inline void raw(char i) {dev<<i;}
    // static inline void raw(unsigned char i) {dev<<i;}
    // static inline void endl() {dev<<std::endl;}
    //.. add more type here
    static inline void endl() {dev<<::endl;}
  };
};//Menu
