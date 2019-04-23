/* -*- C++ -*- */
#pragma once

#include "../base.h"
#include "../printers.h"

namespace AM5 {
  template<
    typename Dev=decltype(Serial),
    Dev& device=Serial,
    typename O=Void<>,
    typename P=DefaultPrinters
  > struct SerialOut:public O {
    using Printers=P;
    using This=SerialOut<Dev,device,O>;
    template<typename Nav,typename T>
    static inline void raw(T o) {device.print(o);}
    static inline void nl() {device.println();}
    static inline void newView() {nl();}
    template<typename Nav>
    static inline void printMenu() {
      Serial<<"SerialOut::printMenu<Nav>()"<<endl;
      Nav::template printMenuRaw<This>();}
  };
};//AM5 namespace
