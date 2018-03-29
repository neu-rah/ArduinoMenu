/* -*- C++ -*- */
/********************
Sept 2014 ~ Oct. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu library for the arduino framework

www.r-site.net

creative commons license 4.0: CC BY-NC-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.
***/

#ifndef RSITE_ARDUINO_MENU_SYSTEM
  #define RSITE_ARDUINO_MENU_SYSTEM
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

  namespace Menu {
    extern const char* numericChars;
    result maxDepthError(menuOut& o,idleEvent e);
  }

  #include "items.h"
  #include "io.h"
  #include "nav.h"

#endif
