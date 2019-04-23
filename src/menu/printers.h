/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  template<typename O>
  struct PanelPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      Nav::template fmtPanel<T,true>();

      using MenuPrinter=typename Nav::Printers::template Menu<O>;
      MenuPrinter::template printMenuRaw<Nav,0>(menu);

      Nav::template fmtPanel<T,false>();
    }
  };

  template<typename O>
  struct MenuPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      Nav::template fmtMenu<T,true>();
      //plugin a printer
      using TitlePrinter=typename Nav::Printers::template Title<O>;
      TitlePrinter::template printMenuRaw<Nav,0>(menu);

      using BodyPrinter=typename Nav::Printers::template Body<O>;
      BodyPrinter::template printMenuRaw<Nav,0>(menu);

      Nav::template fmtMenu<T,false>();
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      Nav::template fmtTitle<T,true,idx>();
      Nav::out(menu);
      Nav::template fmtTitle<T,false>();
    }
  };

  template<typename O>
  struct BodyPrinter:public O {
    template<typename Nav,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      Nav::template fmtMenuBody<T,true>();
      if (Nav::isRange()) {
        //ensure that selection option is withing range
        while(Nav::top()+Nav::posY()>Nav::pos())
          Nav::setTop(Nav::top()-1);
        while(Nav::pos()>=Nav::top()+Nav::freeY())
          Nav::setTop(Nav::top()+1);
      }
      menu.template printItems<Nav,T>();
      Nav::template fmtMenuBody<T,false>();
    }
  };

  template<
    template<typename> class P=PanelPrinter,
    template<typename> class M=MenuPrinter,
    template<typename> class T=TitlePrinter,
    template<typename> class B=BodyPrinter,
    template<typename> class I=Id
  > struct PrintersCfg {
    template<typename O> using Panel=P<O>;
    template<typename O> using Menu=M<O>;
    template<typename O> using Title=T<O>;
    template<typename O> using Body=B<O>;
    template<typename O> using Item=I<O>;
  };

  using DefaultPrinters=PrintersCfg<>;

};//AM5 namespace
