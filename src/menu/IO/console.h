/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu standard C++ stream output

#include "../out.h"
#include <iostream>

using namespace std;

namespace Menu {
  template<ostream& dev=cout,typename O=Void<>>
  struct Console:public O {
    using This=Console<dev,O>;
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(ItemHead& menu) {
      menu.template printTo<NavHead,OutHead,ItemHead,0>();
    }
    template<typename T>
    static inline void raw(T o) {dev<<o;}
    static inline void nl() {cout<<endl;}
  };
}
