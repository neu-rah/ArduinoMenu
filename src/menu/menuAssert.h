/* -*- C++ -*- */
#pragma once

#include <assert.h>
#ifdef ARDUINO

  //from: https://gist.github.com/jlesech/3089916
  #if !defined(__AVR_ATtiny13__) && !defined(ATTINY_CORE)
    #define __ASSERT_USE_STDERR

    // handle diagnostic informations given by assertion and abort program execution:
    void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
        // transmit diagnostic informations through serial link.
        Serial.println(__func);
        Serial.println(__file);
        Serial.println(__lineno, DEC);
        Serial.println(__sexp);
        Serial.flush();
        // abort program execution.
        abort();
    }
  #endif
  
#endif
