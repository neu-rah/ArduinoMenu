/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////////////////////
// printers
struct TextMeasure:public Void<> {
  template<typename T>
  static inline idx_t measure(T o) {
    #ifdef ARDUINO
      return String(o).length();
    #else
      return _str(o);
    #endif
  }
  protected:
    #ifndef ARDUINO
    static inline idx_t _str(const char*o){return std::string(o).length();}
    template<typename T>
    static inline idx_t _str(T i){return std::string(std::to_string(i)).length();}
    #endif
};

template<typename P=TextMeasure>
struct FullPrinter:public P {
  using This=FullPrinter<P>;
  template<typename I,typename Out=This,typename Nav=Drift<>>
  inline static void printMenu(const I& i) {
    trace(MDO<<"+=====================+"<<endl);
    Out::template fmtPanel<true,Out,Nav,I>(0);
    Out::template fmtMenu<true,Out,Nav,I>(0);
    Out::template fmtTitle<true,Out,Nav,I>(0);
    i.template printTo<Out,Nav>();
    Out::template fmtTitle<false,Out,Nav,I>(0);
    Out::template fmtBody<true,Out,Nav,I>(0);

    // if (Out::isRange()) {
    //   //ensure that selection option is withing range
    //   //TODO: option can have variable height...
    //   //this is NOT taking it into account => rework
    //   while(out.top()>nav.pos())
    //     out.setTop(out.top()-1);
    //   while(nav.pos()>=out.top()+out.freeY())
    //     out.setTop(out.top()+1);
    // }

    for(idx_t n=Out::top();n<i.size();n++) {
      if (!Out::freeY()) break;
      P::template clrLine<Out>(P::posY());
      Out::template fmtItem  <true ,Out,Nav,I>(n);
      Out::template fmtIndex <true ,Out,Nav,I>(n);
      Out::template fmtIndex <false,Out,Nav,I>(n);
      Out::template fmtCursor<true ,Out,Nav,I>(n);
      Out::template fmtCursor<false,Out,Nav,I>(n);
      i.template printItem<Out,Nav>(n);
      Out::template fmtItem<false,Out,Nav,I>(n);
    }
    Out::template fmtBody <false,Out,Nav,I>(0);
    Out::template fmtMenu <false,Out,Nav,I>(0);
    Out::template fmtPanel<false,Out,Nav,I>(0);
    trace(MDO<<"+---------------------+"<<endl);
  }
  template<typename I,typename Out=This,typename Nav=Drift<>>
  inline static void printMenu() {printMenu<I,Out,Nav>(I());}
};
