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
  struct Drift:public O {};

  // namespace {
    template<typename Out,typename Data,typename O=Drift<>>
    class NavNodeBase:public O {
      public:
        using This=NavNodeBase<Out,Data,O>;
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
        This::out.template printMenuRaw<This,Out,Data>(This::out,data);
      };
    protected:
      Data data;
  };

  template<typename Out,typename Data,typename O=Drift<>>
  class NavNode:public NavNodeBase<Out,Data,O> {
    public:
      using This=NavNode<Out,Data,O>;
      // inline NavNode() {}
      inline NavNode(Data& item):data(&item) {}
      inline void printMenu() {
        This::out.template printMenuRaw<NavNodeBase<Out,Data,O>,Out,Data>(
          NavNodeBase<Out,Data,O>::out,
          *data
        );
      };
    protected:
      Data* data;
  };

  //base for virtual interface
  struct MenuNavBase {};

  //adapt virtual interface
  template<typename O>
  struct MenuNav:public MenuNavBase {};

};
