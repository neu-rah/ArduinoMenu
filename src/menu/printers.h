/* -*- C++ -*- */
#pragma once

#include "base.h"

namespace AM5 {

  template<typename O>
  struct PanelPrinter:public O {
    template<typename Nav,typename Raw,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      // Serial<<"PanelPrinter::printMenuRaw"<<endl;
      Nav::template fmtPanel<Raw,T,true>();

      using MenuPrinter=typename Raw::Printers::template Menu<O>;
      MenuPrinter::template printMenuRaw<Nav,Raw,0>(menu);

      Nav::template fmtPanel<Raw,T,false>();
    }
  };

  template<typename O>
  struct MenuPrinter:public O {
    template<typename Nav,typename Raw,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      // Serial<<"MenuPrinter::printMenuRaw"<<endl;
      Nav::template fmtMenu<Raw,T,true>();
      //plugin a printer
      using TitlePrinter=typename Raw::Printers::template Title<O>;
      TitlePrinter::template printMenuRaw<Nav,Raw,0>(menu);

      using BodyPrinter=typename Raw::Printers::template Body<O>;
      BodyPrinter::template printMenuRaw<Nav,Raw,0>(menu);

      Nav::template fmtMenu<Raw,T,false>();
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    template<typename Nav,typename Raw,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      // Serial<<"TitlePrinter::printMenuRaw"<<endl;
      Nav::template fmtTitle<Raw,T,true,idx>();
      Nav::out(menu);
      Nav::template fmtTitle<Raw,T,false>();
    }
  };

  template<typename O>
  struct BodyPrinter:public O {
    template<typename Nav,typename Raw,size_t idx,typename T>
    static inline void printMenuRaw(T& menu) {
      // Serial<<"BodyPrinter::printMenuRaw"<<endl;
      Nav::template fmtMenuBody<Raw,T,true>();
      if (Nav::isRange()) {
        //ensure that selection option is withing range
        while(Nav::top()+Nav::posY()>Nav::pos())
          Nav::setTop(Nav::top()-1);
        while(Nav::pos()>=Nav::top()+Nav::freeY())
          Nav::setTop(Nav::top()+1);
      }
      // template<typename Nav,typename Head,size_t idx>
      // template<typename Nav,typename Raw,typename Head,size_t idx>
      menu.template printItems<Nav,Raw,T>();
      Nav::template fmtMenuBody<Raw,T,false>();
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
