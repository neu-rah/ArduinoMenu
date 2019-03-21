/* -*- C++ -*- */
#pragma once

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
