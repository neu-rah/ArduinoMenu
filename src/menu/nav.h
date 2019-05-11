/* -*- C++ -*- */
#pragma once
/**
* @file menu.h
* @author Rui Azevedo
* @date 10 May 2019
* @copyright 2019 Rui Azevedo
* @brief ArduinoMenu navigation implementations
*/

#include "base.h"

template<typename O=Nil> struct Drift:public O {};

template<typename Out,typename Data,typename O=Drift<>>
class StaticNav:public O {
  public:
    using This=StaticNav<Out,Data,O>;
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.template printMenu<This,Out,Data>(*this,out,data);
    }
  protected:
    Out out;
    Data data;
};

template<typename Out,typename Data,typename O=Drift<>>
class DynamicNav:public NavNode,public O {
  public:
    using This=DynamicNav<Out,Data,O>;
    DynamicNav(Data& o):data(&o) {}
    inline void setTarget(Data d) {data=d;}
    inline void printMenu() {
      // data.printItems(out);
      out.template printMenu<This,Out,Data>(*this,out,*data);
    }
  protected:
    Out out;
    Data* data;
};
