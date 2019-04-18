/* -*- C++ -*- */
#pragma once

#ifdef ARDUINO
  #include <streamFlow.h>
#endif


// template<template<typename,typename...> class T,typename O>
// struct Bind {
//   template<typename... OO>
//   struct To:public T<O,OO...> {};
// };

#ifdef ARDUINO
  #include "IO/serialOut.h"
  #include "comp/flashText.h"
#else
  #include "IO/stdOut.h"
#endif

#include "printers.h"
#include "nav.h"
#include "fmt/text.h"
#include "comp/endis.h"
#include "comp/staticText.h"

namespace AM5 {

  // enum class Roles {Panel,Menu,Title,Body,Item,Accel,Cursor,Label,Value,Unit};

  // struct Nil {};

  template<typename O,typename... OO>
  class StaticMenuDataDef:public StaticMenuDataDef<O> {
    public:
      using This=StaticMenuDataDef<O>;
      using Next=StaticMenuDataDef<OO...>;
      constexpr static inline size_t size() {return Next::size()+1;}
      template<typename H,size_t n>
      inline void printItems(H& ph) {
        // Serial<<"print Items "<<n<<" pos:"<<ph.pos()<<endl;
        if (n>=ph.item.top()+H::Printer::height()) return;
        if (n>=ph.item.top()) This::template printItem<H,n>(ph);
        next.template printItems<H,n+1>(ph);
      }

      template<size_t i>
      inline void enable(bool b=true) {
        // Serial<<"StaticMenuDataDef... "<<(b?"enable ":"disable ")<<i<<endl;
        if (i) next.template enable<i-1>(b);
        else O::enable(b);
      }
      template<size_t i>
      inline bool enabled() const {
        // Serial<<"StaticMenuDataDef... enabled "<<i<<endl;
        if (i) return next.template enabled<i-1>();
        else return O::enabled();
      }
    protected:
      static Next next;//being static saves a couple of byte, can be because each item is unique
  };

  template<typename O,typename... OO>
  StaticMenuDataDef<OO...> StaticMenuDataDef<O,OO...>::next;

  template<typename O>
  struct StaticMenuDataDef<O>:public O {
    constexpr static inline size_t size() {return 1;}
    template<typename H,size_t n>
    inline void printItems(H& ph) {
      // Serial<<"printItems->printItem"<<endl;
      printItem<H,n>(ph);}
    template<typename H,size_t n>
    inline void printItem(H& oph) {
      // Serial<<"print item "<<n<<endl;
      PrintHead<typename H::Printer,typename H::Target,n> ph{oph.item};
      ph.template fmtItem<H,true>();
      ph.template fmtIndex<H,true>();
      ph.template fmtCursor<H,true>();
      O::template out<PrintHead<typename H::Printer,typename H::Target,n>>();
      ph.template fmtCursor<H,false>();
      ph.template fmtIndex<H,false>();
      ph.template fmtItem<H,false>();
    }
    template<size_t i>
    inline void enable(bool b=true) {
      if (!i) O::enable(b);
    }
    template<size_t i>
    inline bool enabled() const {
      // Serial<<"StaticMenuDataDef enabled "<<i<<endl;
      if (!i) return O::enabled();
      return true;
    }
  };

  template<typename O>
  struct Cap:public O {
    using This=Cap<O>;
    template<typename Out>
    inline void printMenu() {O::template printMenu<Out>();}
    template<typename Out>
    static inline void out() {O::template out<Out>();}
  };

};
