/* -*- C++ -*- */
#pragma once

#include <sstream>
#include <cstring>
#include "../out.h"

namespace Menu {
  template<typename O=Void>
  struct StringOut:public std::stringstream,O {
    inline void newView() {str("");}
    template<typename T,bool toPrint=true>
    inline void raw(T o) {operator<<(o);}
    template<bool toPrint=true>
    inline void raw<char,toPrint>(char o) {put(o);}
    inline void raw<const char*,toPrint>(const char* o) {write(o,strlen(o));}
  };
};
