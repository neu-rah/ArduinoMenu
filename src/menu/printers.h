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
  inline static void printMenu(const I& i,const Nav& nav,Out& out) {
    _trace(MDO<<"nav pos:"<<nav.pos()<<endl);
    _trace(MDO<<"+=====================+"<<endl);
    Out::template fmtPanel<true,Out,Nav,I>(0,nav);
    // i.template fmt<Panel,true,Out,Nav,I>(0);
    Out::template fmtMenu<true,Out,Nav,I>(0,nav);
    // Out::template fmtTitle<true,Out,Nav,I>(0);
    i.template fmt<Roles::Title,true,I,Out,Nav>(0,nav);
    i.template printTo<Out,Roles::Title,Nav>();
    i.template fmt<Roles::Title,false,I,Out,Nav>(0,nav);
    Out::template fmtBody<true,Out,Nav,I>(0,nav);

    if (Out::isRange()) {
      //ensure that selection option is withing range
      //TODO: option can have variable height...
      //this is NOT taking it into account => rework
      while(Out::top()>nav.pos())
        Out::setTop(Out::top()-1);
      while(nav.pos()>=Out::top()+Out::freeY())
        Out::setTop(Out::top()+1);
    }

    for(idx_t n=Out::top();n<i.size();n++) {
      if (!(Out::inRange(n)&&Out::freeY())) break;
      P::template clrLine<Out>(P::posY());
      if (useItemFmt) {
        i.template fmt<Roles::Prompt,true ,I,Out,Nav>(n,nav);
        i.template fmt<Roles::Index, true ,I,Out,Nav>(n,nav);
        i.template fmt<Roles::Index, false,I,Out,Nav>(n,nav);
        i.template fmt<Roles::Cursor,true ,I,Out,Nav>(n,nav);
        i.template fmt<Roles::Cursor,false,I,Out,Nav>(n,nav);
      } else {
        Out::template fmtItem  <true ,Out,Nav,I>(n,nav);
        Out::template fmtIndex <true ,Out,Nav,I>(n,nav);
        Out::template fmtIndex <false,Out,Nav,I>(n,nav);
        Out::template fmtCursor<true ,Out,Nav,I>(n,nav);
        Out::template fmtCursor<false,Out,Nav,I>(n,nav);
      }
      i.template printItem<Out,Roles::Prompt,Nav>(n);
      if (useItemFmt) i.template fmt<Roles::Prompt,false,I,Out,Nav>(n,nav);
      else Out::template fmtItem<false,Out,Nav,I>(n,nav);
    }
    Out::template fmtBody <false,Out,Nav,I>(0,nav);
    Out::template fmtMenu <false,Out,Nav,I>(0,nav);
    Out::template fmtPanel<false,Out,Nav,I>(0,nav);
    _trace(MDO<<"+---------------------+"<<endl);
  }
  template<typename I,typename Out=This,typename Nav=Drift<>>
  inline static void printMenu() {printMenu<I,Out,Nav>(I());}
};
