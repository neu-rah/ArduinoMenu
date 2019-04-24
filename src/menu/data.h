/* -*- C++ -*- */
#pragma once

namespace AM5 {

  /////////////////////////////////////////////////////////////
  // unlimited size "tuple" (limited by counter size_t)
  // because of its tuple nature we can not map function over it
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
      template<typename Nav,typename Raw,typename Head,size_t idx=0>
      inline void printItems() {
        // Serial<<"StaticMenu::printItems..."<<endl;
        if (Nav::isViewport()) {
          if (!Nav::freeY()) return;//good for viewports
        } else {
          if (idx>=Nav::top()+Nav::freeY()) return;//not for viewports
        }
        // Serial<<"."<<idx<<" "<<Nav::top()<<endl;
        if (idx>=Nav::top()) {
          This::template printItem<Nav,Raw,Head,idx>();
        }
        // Serial<<"StaticMenu::printItems."<<endl;
        next.template printItems<Nav,Raw,Head,idx+1>();
      }
    protected:
      static Next next;
  };

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
    template<typename Nav,typename Raw,typename Head,size_t idx>
    inline void printItems() {
      // Serial<<"StaticMenu::printItems"<<endl;
      if (Nav::isViewport()) {
        if (!Nav::freeY()) return;//for viewports
      } else {
        if (idx>=Nav::top()+Nav::freeY()) return;
      }
      printItem<Nav,Raw,Head,idx>();
    }
    template<typename Nav,typename Raw,typename Head,size_t idx>
    inline void printItem() {
      Nav::template fmtItem<Raw,O,true,idx>();
      Nav::template fmtIndex<Raw,O,true,idx>();
      Nav::template fmtCursor<Raw,O,true,idx>();
      O::template out<Nav,Raw>();
      Nav::template fmtIndex<Raw,O,false,idx>();
      Nav::template fmtCursor<Raw,O,false,idx>();
      Nav::template fmtItem<Raw,O,false,idx>();
    }
  };
};//AM5 namespace
