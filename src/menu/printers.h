/* -*- C++ -*- */
#pragma once

#include "base.h"
namespace Menu {

  template<typename O>
  struct PanelPrinter:public O {
    using This=PanelPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"PanelPrinter::printMenu"<<endl;
      OutHead::template fmtPanel<NavHead,OutHead,ItemHead,true,idx>();

      using MenuPrinter=typename O::Printers::template Menu<O>;
      MenuPrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(menu);
      // menu.template printTo<NavHead,OutHead,ItemHead,idx>();

      OutHead::template fmtPanel<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct MenuPrinter:public O {
    using This=MenuPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"MenuPrinter::printMenu"<<endl;
      OutHead::template fmtMenu<NavHead,OutHead,ItemHead,true,idx>();

      using TitlePrinter=typename O::Printers::template Title<O>;
      TitlePrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(menu);

      OutHead::template fmtMenu<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    using This=TitlePrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"TitlePrinter::printMenu"<<endl;
      OutHead::template fmtTitle<NavHead,OutHead,ItemHead,true,idx>();
      menu.template printTo<NavHead,OutHead,ItemHead,idx>();
      OutHead::template fmtTitle<NavHead,OutHead,ItemHead,false,idx>();
      using BodyPrinter=typename O::Printers::template Body<O>;
      BodyPrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(menu);
    }
  };

  template<typename O>
  struct BodyPrinter:public O {
    using This=BodyPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"BodyPrinter::printMenu"<<endl;
      OutHead::template fmtMenuBody<NavHead,OutHead,ItemHead,true,idx>();
      // if (Nav::isRange()) {
      //   //ensure that selection option is withing range
      //   while(Nav::top()+Nav::posY()>Nav::pos())
      //     Nav::setTop(Nav::top()-1);
      //   while(Nav::pos()>=Nav::top()+Nav::freeY())
      //     Nav::setTop(Nav::top()+1);
      // }
      menu.template printItems<NavHead,OutHead,ItemHead,idx>(menu);
      OutHead::template fmtMenuBody<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct ItemPrinter:public O {
    using This=ItemPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& menu) {
      // cout<<"ItemPrinter::printMenu"<<endl;
      OutHead::template fmtItem<NavHead,OutHead,ItemHead,true,idx>();
      menu.template printItems<NavHead,OutHead,ItemHead,idx>(menu);
      OutHead::template fmtItem<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<
    template<typename> class P=Id,
    template<typename> class M=Id,
    template<typename> class T=Id,
    template<typename> class B=Id,
    template<typename> class I=Id,
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

  template<typename O=PrintersCfg<>>
  struct FullPrinter:public PanelPrinter<
    MenuPrinter<
      TitlePrinter<
        BodyPrinter<
          ItemPrinter<O>
        >
      >
    >
  > {};

  template<typename O=PrintersCfg<>>
  struct SinglePrinter:public PanelPrinter<ItemPrinter<O>> {};

};
