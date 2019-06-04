/* -*- C++ -*- */
#pragma once

#include "base.h"

/////////////////////////////////////////////////////////////////////
template<typename O=Nil>
struct Void:public O {};

template<typename Dev,Dev& dev,typename O=Nil>
struct OutDevice:public O {
  template<typename T>
  static inline void raw(T data) {dev<<data;}
  static inline void nl() {dev<<endl;}
};
template<ostream& dev=cout,typename O=Nil>
struct Console:public OutDevice<ostream,dev,O> {};
using StdOut=Console<>;

struct MenuOut {
  virtual inline void raw(char o)=0;
  virtual inline void raw(const char* o)=0;
  virtual void nl()=0;
};

template<typename O>
struct Output:public MenuOut, public O {
  inline void raw(char o) override {O::raw(o);}
  inline void raw(const char* o) override {O::raw(o);}
  inline void nl() override {O::nl();}
};

// basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& (*pf)(basic_ostream<charT,traits>&))
//
// template<typename T,typename O>
// inline O& p(O& o,T d) {d.print(o);return o;}
//
// template<typename O>
// inline O& p <O,StreamFunction<O>> (O& o,StreamFunction<O> f) {return f(o);}
