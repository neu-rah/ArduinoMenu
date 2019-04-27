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
      inline NavNode() {}
      inline NavNode(Data& item):data(&item) {}
      inline void printMenu() {
        // cout<<"NavNode::printMenu"<<endl;
        //this should pass first by the data object!
        data->template printMenuRaw<This,Out,Data>(*data);
      };
    protected:
      static NavNode<Out,Data,O> nav;
      static Out out;
      Data* data=NULL;//TODO: still test with StaticdNavNode for mem requirements
  };

};
