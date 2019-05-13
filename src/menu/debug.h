/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu output implementations
*/

#ifdef MENU_DEBUG
  #ifdef ARDUINO
    #include <streamFlow.h>
    #ifndef MDO
      #define MDO Serial
    #endif
  #else
    #ifndef MDO
      #define MDO cout
    #endif
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
