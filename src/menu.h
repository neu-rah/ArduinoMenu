/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file
*/

#include "menu/menuIO.h"
#include "menu/printer.h"
#include "menu/nav.h"
#include "menu/item.h"
#include "menu/panels.h"
// #include "menu/imenu.h"

#ifdef MENU_DEBUG
  template<typename Dev,Dev& dev>
  struct DebugOut {
    template<typename O=Void<Nil>>
    struct Part:StaticMenuOut<FullPrinter> {
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

#include <menu/debug.hpp>
#include <menu/item.hpp>
#include <menu/menuIO.hpp>
// #include <menu/imenu.hpp>
