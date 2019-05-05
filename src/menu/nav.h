/* -*- C++ -*- */
#pragma once
////////////////////////////////////////////////////
// Rui Azevedo - Apr2019
// neu-rah (ruihfazevedo@gmail.com)
// ArduinoMenu navigation base

#include "base.h"

namespace Menu {
  //navigation base------------------------------------------------
  template<typename O=Nil>
  struct Drift:public O {
    constexpr static inline size_t size() {return 0;}
    template<typename> constexpr static inline bool _up() {return false;}
    template<typename> constexpr static inline bool _down() {return false;}
    template<typename N> constexpr static inline bool _left() {return N::down();}
    template<typename N> constexpr static inline bool _right() {return N::up();}
    template<typename> constexpr static inline bool _enter() {return false;}
    template<typename> constexpr static inline bool _esc() {return false;}
  };

  // namespace {
    template<typename Out,typename Data,typename O=Drift<>>
    class NavNodeBase:public O {
      public:
        using This=NavNodeBase<Out,Data,O>;
        static inline bool up() {return nav.template _up<This>();}
        static inline bool down() {return nav.template _down<This>();}
        static inline bool left() {return nav.template _left<This>();}
        static inline bool right() {return nav.template _right<This>();}
        static inline bool enter() {return nav.template _enter<This>();}
        static inline bool esc() {return nav.template _esc<This>();}
        template<size_t idx>
        static inline bool enabled() {return true;}
        static inline bool enabled(size_t) {return true;}
        static inline bool selected(size_t) {return false;}
      protected:
        static NavNodeBase<Out,Data,O> nav;
        static Out out;
    };
  // };

  template<typename Out,typename Data,typename O=Drift<>>
  class StaticNavNode:public NavNodeBase<Out,Data,O> {
    public:
      using This=StaticNavNode<Out,Data,O>;
      inline void printMenu() {
        This::out.template printMenuRaw<This,Out,Data>(*this,This::out,data);
      };
      template<size_t idx>
      inline bool enabled() {return data.template enabled<idx>();}
      inline bool enabled(size_t idx) {return data.enabled(idx);}
      template<size_t idx>
      inline void enable(bool o=true) {data.template enable<idx>(o);}
      inline void enable(size_t idx,bool o=true) {data.enable(idx,o);}
      constexpr inline size_t size() {return data.size();}
    protected:
      Data data;
  };

  template<typename Out,typename Data,typename O=Drift<>>
  class NavNode:public NavNodeBase<Out,Data,O> {
    public:
      using This=NavNode<Out,Data,O>;
      inline NavNode() {}
      inline NavNode(Data& item):data(&item) {}
      inline void printMenu() {
        This::out.template printMenuRaw<NavNodeBase<Out,Data,O>,Out,Data>(
          *this,
          NavNodeBase<Out,Data,O>::out,
          *data
        );
      };
      template<size_t idx>
      inline bool enabled() {return data->template enabled<idx>();}
      inline bool enabled(size_t idx) {return data->enabled(idx);}
      template<size_t idx>
      inline void enable(bool o=true) {data->template enable<idx>(o);}
      inline void enable(size_t idx,bool o=true) {data->enable(idx,o);}
      inline size_t size() {return data->size();}
    protected:
      Data* data;
  };

  //base for virtual interface
  struct MenuNavBase {};

  //adapt virtual interface
  template<typename O>
  struct MenuNav:public MenuNavBase {};

  template<typename O=Drift<>>
  class PosDef:public O {
    public:
      template<size_t idx>
      static inline bool selected() {return at==idx;}
      template<typename Nav>
      static inline bool _up() {
        if (at<Nav::size()-1) {at++;return true;}
        return O::template _up<Nav>();
      }
      template<typename Nav>
      static inline bool _down() {
        if (at>0) {at--;return true;}
        return O::template _down<Nav>();
      }
      static inline size_t pos() {return at;}
    protected:
      static size_t at;
  };

};
