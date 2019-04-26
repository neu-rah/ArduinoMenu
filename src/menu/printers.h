/* -*- C++ -*- */
#pragma once

#include "base.h"
namespace Menu {

  template<typename O>
  struct ItemPrinter:public O {
    using This=ItemPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"ItemPrinter::printMenu"<<endl;
      This::template fmtItem<NavHead,OutHead,ItemHead,true,idx>();
      menu.template printTo<NavHead,OutHead,ItemHead,idx>();
      This::template fmtItem<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<
    template<typename> class P=Id,
    template<typename> class M=Id,
    template<typename> class T=Id,
    template<typename> class B=Id,
    template<typename> class I=ItemPrinter,
    typename O=Void<>
  > struct PrintersCfg:public O {
    struct Printers {
      template<typename X> using Panel=P<X>;
      template<typename X> using Menu=M<X>;
      template<typename X> using Title=T<X>;
      template<typename X> using Body=B<X>;
      template<typename X> using Item=I<X>;
    };
  };

};
