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
  // template<typename I,typename Out=This,typename Nav=Drift<>>
  // inline static void printMenu(const I& i,const Nav& nav,Out& out) {
  //   trace(MDO<<"nav pos:"<<nav.pos()<<endl);
  //   trace(MDO<<"+=====================+"<<endl);
  //   out.template fmtPanel<true,I,Out,Nav>(0,nav,out);
  //   // i.template fmt<Panel,true,Out,Nav,I>(0);
  //   out.template fmtMenu<true,I,Out,Nav>(0,nav,out);
  //   // out.template fmtTitle<true,Out,Nav,I>(0);
  //   i.template fmt<Roles::Title,true,I,Out,Nav>(0,nav,out);
  //   i.template printTo<Out,Roles::Title,Nav>();
  //   i.template fmt<Roles::Title,false,I,Out,Nav>(0,nav,out);
  //   out.template fmtBody<true,I,Out,Nav>(0,nav,out);
  //
  //   if (out.isRange()) {
  //     //ensure that selection option is withing range
  //     //TODO: option can have variable height...
  //     //this is NOT taking it into account => rework
  //     while(out.top()>nav.pos())
  //       out.setTop(out.top()-1);
  //     while(nav.pos()>=out.top()+out.freeY())
  //       out.setTop(out.top()+1);
  //   }
  //
  //   for(idx_t n=out.top();n<i.size();n++) {
  //     if (!out.freeY()) break;
  //     // if (!(out.inRange(n)&&out.freeY())) break;
  //     P::template clrLine<Out>(P::posY());
  //     if (useItemFmt) {
  //       i.template fmt<Roles::Prompt,true ,I,Out,Nav>(n,nav,out);
  //       i.template fmt<Roles::Index, true ,I,Out,Nav>(n,nav,out);
  //       i.template fmt<Roles::Index, false,I,Out,Nav>(n,nav,out);
  //       i.template fmt<Roles::Cursor,true ,I,Out,Nav>(n,nav,out);
  //       i.template fmt<Roles::Cursor,false,I,Out,Nav>(n,nav,out);
  //     } else {
  //       out.template fmtItem  <true ,I,Out,Nav>(n,nav,out);
  //       out.template fmtIndex <true ,I,Out,Nav>(n,nav,out);
  //       out.template fmtIndex <false,I,Out,Nav>(n,nav,out);
  //       out.template fmtCursor<true ,I,Out,Nav>(n,nav,out);
  //       out.template fmtCursor<false,I,Out,Nav>(n,nav,out);
  //     }
  //     i.template printItem<Out,Roles::Prompt,Nav>(n);
  //     if (useItemFmt) i.template fmt<Roles::Prompt,false,I,Out,Nav>(n,nav,out);
  //     else out.template fmtItem<false,I,Out,Nav>(n,nav,out);
  //   }
  //   out.template fmtBody <false,I,Out,Nav>(0,nav,out);
  //   out.template fmtMenu <false,I,Out,Nav>(0,nav,out);
  //   out.template fmtPanel<false,I,Out,Nav>(0,nav,out);
  //   trace(MDO<<"+---------------------+"<<endl);
  // }
  // template<typename I,typename Out=This,typename Nav=Drift<>>
  // inline static void printMenu() {printMenu<I,Out,Nav>(I());}
};
