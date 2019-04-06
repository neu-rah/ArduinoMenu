/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// use arduino Serial as menu output

#include <Arduino.h>
#include <streamFlow.h>
#include "../out.h"
#include "../nav.h"
#include "../panels.h"
#include "../fmt/textCursor.h"
#include "../fmt/cursorPos.h"
#include "../fmt/titleWrap.h"
#include "../printers.h"

namespace Menu {

  template<typename P,decltype(Serial)& dev=Serial, typename O=Void>
  struct SerialOutDef:public O {
    using RAW_DEVICE = SerialOutDef<P,dev,O>;
    using Parts=P;
    // using MUST_BE_AT_OUTPUT_BASE=O::OUTPUT_BASE;//or maybe not
    template<typename T>
    inline void raw(T i) {dev.print(i);}
    static inline void endl() {dev<<::endl;O::endl();}//we use no viewport
  };

  using SerialParts=DeviceParts<
    Chain<TextCursorPrinter,ItemPrinter>::To,//emit format messages for accel, cursor amd item
    TitlePrinter//emit format messages for titles (fmtTitle)
  >;

  template<template<typename> class N=NavNode>
  using SerialFmt = Menu::Chain<//wrap inner types
    DebugFmt,//add debug info when enabled
    TextCursorFmt,//signal selected option on text mode
    CursorPosFmt,//cursor control, change line at item end
    TitleWrap,//wrap title in []
    TitlePrinter,
    FullPrinter,//print inner then options
    N//flat navigation control (no sub menus)
  >;

};//Menu

template<decltype(Serial)& dev=Serial,typename Parts=Menu::SerialParts>
using SerialOutDev=Menu::SerialOutDef<Parts,Serial>;
