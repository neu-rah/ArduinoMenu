/* -*- C++ -*- */
#pragma once

#include <Arduino.h>
// #define MENU_ASYNC
// #define MENU_FMT_WRAPS
// #define USE_PGM
// #define USE_RAM
// #define MENU_DEBUG

#include "macros.h"
#include "menuBase.h"
#include "shadows.h"

using namespace Menu;

#ifdef MENU_DEBUG
  String& operator<<(String&s,prompt &p);
  Stream& operator<<(Stream&o,prompt& p);
  Print& operator<<(Print&o,prompt& p);
#endif

#if defined(MENU_DEBUG) || defined(MENU_ASYNC)
  #ifndef ios_h
    #ifndef ARDUINO_STREAMING
      //https://github.com/neu-rah/streamFlow
      #include <streamFlow.h>
    #else
      //https://github.com/scottdky/Streaming
      #include <Streaming.h>
    #endif
  #endif
#endif


#if defined(MENU_DEBUG) && defined(TRACE) && !defined(trace)
  #define trace(x) x
#else
  #define trace(x)
#endif
#if defined(MENU_DEBUG) && !defined(_trace)
  #define _trace(x) x
#else
  #ifndef _trace
    #define _trace(x)
  #endif
#endif
#if defined(MENU_RELEASE) && !defined(__trace)
  #define __trace(x) x
#else
  #ifndef __trace
    #define __trace(x)
  #endif
#endif

namespace Menu {
  extern const char* numericChars;
  result maxDepthError(menuOut& o,idleEvent e);
}

#include "items.h"
#include "menuIo.h"
#include "nav.h"
