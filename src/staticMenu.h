/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file for static definitions
*/

// #include "menu/path.h"
// #include "menu/tags.h"
#include "menu/staticIO.h"
#include "menu/printer.h"

namespace Menu {
  #ifdef MENU_DEBUG
    template<typename Dev,Dev& dev>
    struct DebugOut {
      template<typename O=Void<Nil>>
      struct Part:StaticMenuOut<FullPrinter::Part> {
        template<bool toPrint=true>
        inline static void nl() {dev.println();}
        template<typename T,bool toPrint=true>
        inline static void raw(const T o,Tag role=Tag::Raw) {
          #ifdef ARDUINO
            dev.print(o);
          #else
            dev<<o;
          #endif
        }
      };
    };

    #ifdef ARDUINO
      using MenuDebug=DebugOut<decltype(Serial),Serial>::Part<>;
    #else
      using MenuDebug=DebugOut<decltype(std::cout),std::cout>::Part<>;
    #endif
    extern MenuDebug debugOut;

    inline MenuDebug& operator<<(MenuDebug& o,const char* t) {o.raw(t);return o;}
    inline MenuDebug& operator<<(MenuDebug& o,unsigned char t) {o.raw(t);return o;}
  #endif
};

#include "menu/staticNav.h"
#include "menu/staticItem.h"
#include "menu/panels.h"
#include "menu/tags.h"

// must be at end of file
#include <menu/debug.hpp>
#include <menu/staticItem.hpp>
