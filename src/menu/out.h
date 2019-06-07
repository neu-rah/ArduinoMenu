/* -*- C++ -*- */
#pragma once

#include "base.h"

//////////////////////////////////////////////
template<typename O>
struct Void:public O {
  template<typename T>
  static inline void raw(T o) {}
  template<typename N,typename H,typename I>
  static void print(N,H,I) {}
};

template<typename Dev,Dev& dev,typename O=Void<>>
struct StaticOut:public O {
  template<typename T>
  inline void raw(T o) {dev<<o;}
  template<typename N,typename H,typename I>
  inline void print(N n,H o,I i) {i.print(n,o);}
};

template<ostream& dev,typename O=Void<>>
using Console=StaticOut<ostream,dev,O>;

template<typename O=Void<>>
using StdOut=Console<cout,O>;

struct IMenuOut {
  virtual void raw(const char* o)=0;
  virtual inline void print(INavNode& n,IItem& i)=0;
};

template<typename O>
struct MenuOut:public O {
  void raw(const char* o) override {O::raw(o);}
  void print(INavNode& n,IItem& i) override {i.print(n,*this,i);}
};
