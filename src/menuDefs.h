/* -*- C++ -*- */

#pragma once

// #ifndef RSITE_ARDUINO_MENU_SYSTEM
//   #define RSITE_ARDUINO_MENU_SYSTEM
  #include <Arduino.h>
  // #define MENU_ASYNC
  // #define MENU_FMT_WRAPS

  #include "menuBase.h"
  #include "shadows.h"

  using namespace Menu;

  #ifdef MENU_DEBUG
    String& operator<<(String&s,prompt &p);
    Stream& operator<<(Stream&o,prompt& p);
    Print& operator<<(Print&o,prompt& p);
  #endif

  #if defined(MENU_DEBUG) || defined(MENU_ASYNC)
    #ifndef ARDUINO_STREAMING
      #include <streamFlow.h>
    #else
      #include <Streaming.h>
    #endif
  #endif


  #if defined(MENU_DEBUG) && defined(TRACE)
    #define trace(x) x
  #else
    #define trace(x)
  #endif
  #ifdef MENU_DEBUG
    #define _trace(x) x
  #else
    #define _trace(x)
  #endif
  #ifndef MENU_RELEASE
    #define __trace(x) x
  #else
    #define __trace(x)
  #endif

  namespace Menu {
    extern const char* numericChars;
    result maxDepthError(menuOut& o,idleEvent e);
  }

  #include "items.h"
  #include "menuIo.h"
  #include "nav.h"
// #endif
