/* -*- C++ -*- */
#pragma once

#include <sstream>
#include <cstring>
#include "../out.h"

template<typename O=Void>
struct StringOut:public std::stringstream,O {
  inline void newView() {str("");}
  template<typename Out,boool toPrint=true>
  inline void nl(Out&) {write("\r\n",2);}
  template<typename T>
  inline void raw(T o,Roles role=Roles::Raw) {operator<<(o);}
  inline void raw(char o,Roles role=Roles::Raw) {put(o);}
  inline void raw(const char* o,Roles role=Roles::Raw) {write(o,strlen(o));}
};
