/* -*- C++ -*- */
#pragma once

#include "base.h"
#include "nav.h"
#include "out.h"
#include "comp/endis.h"

namespace AM5 {

  ////////////////////////////////////////////////////////////
  // menu items
  template<typename O=Nil>
  struct Empty:public O {};

  template<const char**text,typename O=Empty<>>
  struct StaticText:public O {
    template<typename N>
    static inline void out() {
      N::raw(text[0]);
      // nav.out.raw(reinterpret_cast<const __FlashStringHelper *>(text[0]));
    }
  };

  /////////////////////////////////////////////////////////////
  // unlimited size "tupple" (limited by counter size_t)
  // because of its tupple nature we can not map function over it
  template<typename O,typename... OO>
  class  StaticMenu:public StaticMenu<O> {
    public:
      using This=StaticMenu<O>;
      using Next=StaticMenu<OO...>;

      constexpr static inline size_t size() {return Next::size()+1;}
      template<size_t n>
      inline bool enabled() const {
        return n?next.template enabled<n-1>():O::enabled();
      }
      template<size_t n>
      inline void enable(bool o) {
        return n?next.template enable<n-1>(o):O::enable(o);
      }
      template<typename Head,size_t idx=0>
      inline void printItems() {
        This::template printItem<Head,idx>();
        next.template printItems<Head,idx+1>();
      }
      // template<typename Head,size_t n>
      // inline void printItem() {
      //   // cout<<"print item... "<<n<<endl;
      //   if (n) next.template printItem<Head,n-1>();
      //   else {
      //     O::template out<Head>();
      //   }
      // }
    protected:
      static Next next;
  };

  template<typename O,typename... OO>
  StaticMenu<OO...> StaticMenu<O,OO...>::next;

  template<typename O>
  struct StaticMenu<O>:public O {
    constexpr static inline size_t size() {return 1;}
    template<size_t n>
    inline bool enabled() const {
      return n?true:O::enabled();
    }
    template<size_t n>
    inline void enable(bool o) {
      if(!n) O::enable(o);
    }
    template<typename Head,size_t idx=0>
    inline void printItems() {printItem<Head,idx>();}
    template<typename Head,size_t idx>
    inline void printItem() {
      // cout<<"print item "<<n<<endl;
      // if (!n) {
        // Head::rawOut.template fmtItem<Head,true>();
        Head::template fmtItem<O,true,idx>();
        Head::template fmtIndex<O,true,idx>();
        Head::template fmtCursor<O,true,idx>();
        O::template out<Head>();
        Head::template fmtIndex<O,false,idx>();
        Head::template fmtCursor<O,false,idx>();
        Head::template fmtItem<O,false,idx>();
      // }
    }
  };
};//AM5 namespace

#include "nav.hpp"
