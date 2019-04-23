/* -*- C++ -*- */
#pragma once

#include "../base.h"
#include "../printers.h"

namespace AM5 {
  template<
    ostream& device=std::cout,
    typename O=Void<>,
    typename P=DefaultPrinters
  > struct StdOut:public O {
    using Printers=P;
    using This=StdOut<device,O>;
    template<typename Nav,typename T>
    static inline void raw(T o) {device<<o;}
    static inline void nl() {device<<std::endl;}
    template<typename Nav>
    static inline void printMenu() {Nav::template printMenuRaw<This>();}
  };
};//AM5 namespace
