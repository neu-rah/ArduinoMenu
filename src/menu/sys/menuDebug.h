/* -*- C++ -*- */
#pragma once

#include "menu/sys/kernel/debug.h"
#include "menu/sys/kernel/base.h"

namespace Menu {
  #if defined(RS_DEBUG)&&!defined(RELEASE)
    #ifdef ARDUINO
      struct DebugOut {
        template<typename Data>
        static void print(Data o) {Serial.print(o);}
        static void nl() {Serial.println();}
        template<Fmt,bool> static void fmt(int n=0,bool sel=false) {}
        template<Fmt tag> void fmtStart(int n=0,bool sel=false) {fmt<tag,true>(n,sel);}
        template<Fmt tag> void fmtStop(int n=0,bool sel=false) {fmt<tag,false>(n,sel);}
      };
    #else
      #include <iostream>
      struct DebugOut {
        template<typename Data>
        static void print(Data o) {std::cout<<o;}
        static void nl() {std::cout<<std::endl;}
        template<Fmt,bool> static void fmt(int n=0,bool sel=false) {}
        template<Fmt tag> void fmtStart(int n=0,bool sel=false) {fmt<tag,true>(n,sel);}
        template<Fmt tag> void fmtStop(int n=0,bool sel=false) {fmt<tag,false>(n,sel);}
      };
    #endif
    // template<typename Out,typename Data>
    // DebugOut& operator<<(Out& out,Data data) {out.print(data);return out;}
    using MDO=DebugOut;

    extern MDO mdo;
    inline void ptrIs(){}
    template<typename O,typename... OO> 
      inline void ptrIs(const O& o,OO... oo)
      {mdo<<&o<<" ";ptrIs(oo...);}

  #endif

};