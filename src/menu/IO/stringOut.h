/* -*- C++ -*- */
#pragma once

#include <sstream>
#include <cstring>
#include "../out.h"

template<typename O=Void>
struct StringOut:public std::stringstream,O {
  inline void newView() {str("");}
  inline void nl() {write("\r\n",2);}
  template<typename T>
  inline void raw(T o) {operator<<(o);}
  inline void raw(char o) {put(o);}
  inline void raw(const char* o) {write(o,strlen(o));}
};
