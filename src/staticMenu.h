/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file for static definitions
*/

#include "menu/staticIO.h"
#include "menu/printer.h"
#include "menu/staticNav.h"
#include "menu/staticItem.h"
#include "menu/panels.h"
// #include "menu/imenu.h"

namespace Menu {
  #ifdef MENU_DEBUG
    template<typename Dev,Dev& dev>
    struct DebugOut {
      template<typename O=Void<Nil>>
      struct Part:StaticMenuOut<FullPrinter::Part> {
        // template<bool toPrint=true>
        inline static void nl() {dev.println();}
        template<typename T>
        inline static void raw(T o,Roles role=Roles::Raw) {
          #ifdef ARDUINO
            dev.print(o);
          #else
            dev<<o;
          #endif
        }
      };
    };

    #ifdef ARDUINO
      extern DebugOut<decltype(Serial),Serial>::Part<> debugOut;
    #else
      // #include <iostream>
      extern DebugOut<decltype(std::cout),std::cout>::Part<> debugOut;
    #endif
  #endif
};

#include <menu/debug.hpp>
#include <menu/staticItem.hpp>
