/* -*- C++ -*- */
#pragma once

#include "base.h"

////////////////////////////////////
template<typename N>
struct Drift:public N {
  template<typename Out> static inline void printMenu(Out& out) {}
  template<typename Out> static inline void newView() {Out::nl();}
  constexpr static inline idx_t pos() {return 0;}
  constexpr static inline idx_t size() {return 0;}
  constexpr static inline bool selected(idx_t) {return false;}
  constexpr static inline bool enabled(idx_t) {return true;}
  constexpr static inline Modes mode() {return Modes::Normal;}
};

template<typename Data,typename N>
class StaticNav:public N {
  public:
    template<typename Out>
    inline void printMenu(Out& out) {out.printMenu(*this,out,data);}
    inline idx_t size() {return data.size();}
  protected:
    Data data;
};

template<typename N>
class NavPos:public N {
  public:
    inline idx_t pos() {return at;}
    inline bool selected(idx_t idx) const {return at==idx;}
  protected:
    idx_t at;
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
