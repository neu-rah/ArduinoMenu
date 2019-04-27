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

  template<typename Out,typename Data,typename O=Drift<>>
  class NavNode:public O {
    public:
      using This=NavNode<Out,Data,O>;
      static inline void printMenu() {
        // cout<<"NavNode::printMenu"<<endl;
        Out::template printMenuRaw<This,Out,Data>(menu);
      };
    protected:
      static NavNode<Out,Data,O> nav;
      static Out out;
      static Data menu;
  };

};
