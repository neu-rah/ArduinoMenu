/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu menu item base and some common items

#include "base.h"

namespace Menu {
  //structure item base--------------------------------------------------
  template<typename O=Nil>
  struct Empty {
    // template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    // static inline void printTo() {}
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printItems(ItemHead& item) {
      item.template printTo<NavHead,OutHead,ItemHead,idx>();
    }
  };

  template<const char** text,typename O=Empty<>>
  struct StaticText:public O {
    using This=StaticText<text,O>;
    template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
    static inline void printTo() {
      // cout<<"StaticText::printTo"<<endl;
      OutHead::raw(text[0]);
    }
  };

  template<typename O,typename... OO>
  class StaticList:public StaticList<O> {
    public:
      using This=StaticList<O>;
      using Next=StaticList<OO...>;
      template<typename NavHead,typename OutHead,typename ItemHead,idx_t idx>
      static inline void printItems(ItemHead& item) {
        // cout<<"StaticList...::printItems"<<endl;
        using ItemPrinter=typename OutHead::Printers::template Item<This>;
        OutHead::template fmtItem<NavHead,OutHead,ItemPrinter,true,idx>();
        ItemPrinter::template printTo<NavHead,OutHead,This,idx>();
        OutHead::template fmtItem<NavHead,OutHead,ItemPrinter,false,idx>();
        next.template printItems<NavHead,OutHead,Next,idx>(next);
      };
    protected:
      static Next next;
  };

  template<typename O,typename... OO>
  StaticList<OO...> StaticList<O,OO...>::next;

  template<typename O>
  struct StaticList<O>:public O {
    using This=StaticList<O>;
  };

};
