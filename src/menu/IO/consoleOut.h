/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use std console as menu output

#include <ostream>
using namespace std;

#include "../out.h"
#include "../printers.h"
#include "../nav.h"
#include "../fmt/text.h"
#include "../fmt/textCursor.h"
#include "../fmt/titleWrap.h"
#include "../fmt/index.h"

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

  using ConsoleParts=DeviceParts<
    Chain<TextAccelPrinter,TextCursorPrinter,ItemPrinter>::To//emit format messages for accel, cursor amd item
    // ,TitlePrinter//emit format messages for titles (fmtTitle)
  >;

  template<template<typename> class N=NavNode>
  using ConsoleFmt = Menu::Chain<//wrap inner types
    // DebugFmt,//add debug info when enabled
    IndexFmt,//print option index (1-9)
    TextCursorFmt,//signal selected option on text mode
    TextFmt,//normal text format
    TitleWrap,//wrap title in []
    TitlePrinter,
    FullPrinter,//print inner then options
    N//flat navigation control (no sub menus)
  >;

};//Menu

template<ostream& dev=cout,typename Parts=Menu::ConsoleParts>
using ConsoleOutDev=Menu::ConsoleOutDef<Parts,cout>;
