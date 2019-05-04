/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu standard C++ stream output

#include "../out.h"

using namespace std;

namespace Menu {
  template<typename Dev=decltype(Serial),Dev& dev=Serial,typename O=Void<>>
  struct SerialOut:public O {
    using This=SerialOut<Dev,dev,O>;
    template<typename NavHead,typename OutHead,typename ItemHead>
    static inline void printMenuRaw(OutHead& out,ItemHead& item) {
      out.template printMenu<NavHead,OutHead,ItemHead,0>(out,item);
    }
    template<typename T>
    static inline void raw(T o) {dev.print(o);}
    static inline void nl() {dev.println();}
  };
}
