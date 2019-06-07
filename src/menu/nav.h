/* -*- C++ -*- */
#pragma once

#include "base.h"

////////////////////////////////////
template<typename N>
struct Drift:public N {
  template<typename Out>
  static inline void printMenu(Out& out) {}
};

template<typename Data,typename N>
class StaticNav:public N {
  public:
    template<typename Out>
    inline void printMenu(Out& out) {out.print(*this,out,data);}
  protected:
    Data data;
};

template<typename N>
class NavPos:public N {
  public:
  protected:
    idx_t sel;
};

template<typename Data,typename N>
class StaticNavTree:public StaticNav<N> {
};

struct INavNode {
  virtual inline void printMenu(IMenuOut& out)=0;
};

template<typename N=Nil>
struct NavNode:public INavNode,public N {
  inline void printMenu(IMenuOut& out) override {N::print(out);}
};
