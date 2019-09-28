/* -*- C++ -*- */
#pragma once
#include "api.h"

template<typename Idx=Idx,typename N=Drift<>>
struct NavPos:N {
  template<typename It,typename Nav,typename Out>
  inline static void print(It& it,Nav& nav,Out& out,Ref ref,Idx n) {
    it.template print<It,Nav,Out>(it,nav,out,ref,n);
  }
  inline Idx pos() const {return at;}
  Idx at=0;
};

template<typename Data,Idx max_depth,typename N=Drift<>>
struct StaticNavTree:N {
  inline StaticNavTree(Data& o):data(o) {}
  operator Ref() {return {level,path};}
  inline Idx cur() const {return level?path[level]:N::pos();}

  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    Ref ref=*this;
    data.template print<Data,Nav,Out>(data,nav,out,ref,cur());
  }

  template<typename Nav>
  inline bool enter(Nav& nav) {data.template enter<Data,Nav>(data,nav,*this,cur());}

  Idx level=0;
  Idx path[max_depth];
  Data& data;
};

template<typename N>
struct StaticRoot:N {
  using N::N;
  using This=StaticRoot<N>;

  template<typename Out> inline void print(Out& o) {N::template print<This,Out>(*this,o);}
  template<typename Out> inline void print() {
    Out out;
    This::template print<Out>(out);
  }

  inline bool enter() {return N::template enter<This>(*this);}
  inline bool esc() {return false;}
  inline bool up() {return false;}
  inline bool down() {return false;}
};
