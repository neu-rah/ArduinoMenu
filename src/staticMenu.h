/* -*- C++ -*- */
#pragma once
/**
* @author Rui Azevedo
* @brief ArduinoMenu main include file

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

// #include "menu/path.h"
// #include "menu/tags.h"
#include "menu/staticIO.h"
#include "menu/sys/printer.h"

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

#include "menu/sys/kernel/tags.h"
#include "menu/sys/panels.h"
#include "menu/sys/parsers.h"
#include "menu/staticNav.h"
#include "menu/staticItem.h"

// must be at end of file
#include "menu/sys/kernel/base.hpp"
#include "menu/sys/kernel/debug.hpp"
// #include "menu/staticItem.hpp"
