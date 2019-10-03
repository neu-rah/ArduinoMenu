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
  inline bool selected(Idx idx) const {return at==idx;}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"NavPos::cmd"<<endl);
    switch(c) {
      case Cmds::Up:
        if (at<it.size()-1) at++;
        break;
      case Cmds::Down:
        if (at>0) at--;
        break;
    }
    N::template _cmd<c,It,Nav>(it,nav);
  }
  Idx at=0;
};

template<typename Data,Idx max_depth,typename N=Drift<>>
struct StaticNavTree:N {
  inline StaticNavTree(Data& o):data(o) {}
  operator Ref() {return {level,path};}
  inline Idx cur() const {return level?path[level]:N::pos();}

  template<typename Nav,typename Out>
  inline void print(Nav& nav,Out& out) {
    _trace(MDO<<"StaticNavTree::print level:"<<level<<endl);
    Ref ref=*this;
    // data.template printMenu<Data,Nav,Out>(data,nav,out);
    data.template printMenu<Data,Nav,Out>(data,nav,out,ref,cur());
  }

  template<Cmds c,typename Nav>
  inline bool cmd(Nav& nav) {
    data.template cmd<c,Data,Nav>(data,nav,*this,cur());
  }

  inline void open() {
    if (level>=max_depth-1) return;
    trace(MDO<<"level:"<<level<<" max:"<<max_depth<<endl);
    assert(level<max_depth-1);
    path[++level]=0;
    trace(MDO<<"level:"<<level<<" max:"<<max_depth<<endl);
  }

  Idx level=0;
  Idx path[max_depth];
  Data& data;
};

template<typename N>
struct StaticRoot:N {
  using N::N;
  using This=StaticRoot<N>;

  template<typename Out> inline void print(Out& o) {
    N::template print<This,Out>(*this,o);
  }
  template<typename Out> inline void print() {
    Out out;
    This::template print<Out>(out);
  }

  inline bool enter() {return N::template cmd<Cmds::Enter,This>(*this);}
  inline bool esc() {return N::template cmd<Cmds::Esc,This>(*this);}
  inline bool up() {return N::template cmd<Cmds::Up,This>(*this);}
  inline bool down() {return N::template cmd<Cmds::Down,This>(*this);}
  inline bool left() {return N::template cmd<Cmds::Left,This>(*this);}
  inline bool right() {return N::template cmd<Cmds::Right,This>(*this);}
};
