/* -*- C++ -*- */
#pragma once

#include "base.h"
#include "out.h"
namespace Menu {

  template<typename O>
  struct PanelPrinter:public O {
    using This=PanelPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& item) {
      // cout<<"PanelPrinter::printMenu"<<endl;
      OutHead::template fmtPanel<NavHead,OutHead,ItemHead,true,idx>();

      using MenuPrinter=typename O::Printers::template Menu<O>;
      MenuPrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(item);

      OutHead::template fmtPanel<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct MenuPrinter:public O {
    using This=MenuPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& item) {
      // cout<<"MenuPrinter::printMenu"<<endl;
      OutHead::template fmtMenu<NavHead,OutHead,ItemHead,true,idx>();

      using TitlePrinter=typename O::Printers::template Title<O>;
      TitlePrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(item);

      OutHead::template fmtMenu<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct TitlePrinter:public O {
    using This=TitlePrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& item) {
      // cout<<"TitlePrinter::printMenu"<<endl;
      OutHead::template fmtTitle<NavHead,OutHead,ItemHead,true,idx>();
      item.template printItem<NavHead,OutHead,ItemHead,idx>();
      OutHead::template fmtTitle<NavHead,OutHead,ItemHead,false,idx>();
      using BodyPrinter=typename O::Printers::template Body<O>;
      BodyPrinter::template printMenu<NavHead,OutHead,ItemHead,idx>(item);
    }
  };

  template<typename O>
  struct BodyPrinter:public O {
    using This=BodyPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& item) {
      // cout<<"BodyPrinter::printMenu"<<endl;
      OutHead::template fmtMenuBody<NavHead,OutHead,ItemHead,true,idx>();
      // if (Nav::isRange()) {
      //   //ensure that selection option is withing range
      //   while(Nav::top()+Nav::posY()>Nav::pos())
      //     Nav::setTop(Nav::top()-1);
      //   while(Nav::pos()>=Nav::top()+Nav::freeY())
      //     Nav::setTop(Nav::top()+1);
      // }
      item.template printItems<NavHead,OutHead,ItemHead,idx>(item);
      OutHead::template fmtMenuBody<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  template<typename O>
  struct ItemPrinter:public O {
    using This=ItemPrinter<O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printMenu(ItemHead& item) {
      // cout<<"ItemPrinter::printMenu"<<endl;
      OutHead::template fmtItem<NavHead,OutHead,ItemHead,true,idx>();
      item.template printItems<NavHead,OutHead,ItemHead,idx>(item);
      OutHead::template fmtItem<NavHead,OutHead,ItemHead,false,idx>();
    }
  };

  //printer (parts) config
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

  //full printer (all parts)
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

  //single item printer
  template<typename O=PrintersCfg<>>
  struct SinglePrinter:public PanelPrinter<ItemPrinter<O>> {};

};
