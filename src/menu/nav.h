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
  inline void setPos(Idx n) {at=n;}
  inline bool selected(Idx idx) const {return at==idx;}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"NavPos::_cmd "<<c<<endl);
    switch(c) {
      case Cmds::Up:
        trace(MDO<<"at:"<<at<<" size:"<<it.size((Ref)nav)<<endl);
        if (at<(it.size((Ref)nav)-1)) at++;
        else return false;
        return true;
      case Cmds::Down:
        if (at>0) at--;
        else return false;
        return true;
    }
    return N::template _cmd<c,It,Nav>(it,nav);
  }
  Idx at=0;
};

template<typename Data,Idx max_depth,typename N=Drift<>>
struct StaticNavTree:N {
  inline StaticNavTree(Data& o):data(o) {}
  operator Ref() {return {level,path};}
  inline Idx cur() const {return level?path[level]:N::pos();}
  inline Idx size() {return size(operator Ref());}
  inline Idx size(Ref ref) {return data.size(ref);}

  template<typename Nav,typename Out>
  inline void print(Nav& nav) {
    trace(MDO<<"StaticNavTree::print level:"<<level<<endl);
    Ref ref=*this;
    data.template printMenu<Data,Nav,Out>(data,nav,ref,ref.len?ref.head():cur());
  }

  template<Cmds c,typename Nav>
  inline bool cmd(Nav& nav) {
    trace(MDO<<"NavPos::cmd "<<c<<endl);
    if (c==Cmds::Esc) {
      trace(MDO<<"NavPos closing!!! "<<c<<endl);
      close();
      return true;
    }
    // Ref ref=*this;
    // return data.template cmd<c,Data,Nav>(data,nav,ref,ref.len?ref.head():N::pos());
    return data.template cmd<c,Data,Nav>(data,nav,*this,cur());
  }

  inline void open() {
    if (level>=max_depth-1) return;
    trace(MDO<<"level:"<<level<<" max:"<<max_depth<<endl);
    assert(level<max_depth-1);
    path[level++]=N::pos();
    // path[level]=0;
    N::setPos(0);
    trace(MDO<<"level:"<<level<<" at:"<<path[level]<<" max:"<<max_depth<<endl);
  }

  inline void close() {
    trace(MDO<<"exit from level:"<<level<<endl);
    if (level>0) {
      level--;
      N::setPos(path[level]);
    }
  }

  Idx level=0;
  Idx path[max_depth];
  Data& data;
};

template<typename N>
struct StaticRoot:N {
  using N::N;
  using This=StaticRoot<N>;

  using N::print;
  template<typename Out> inline void print() {
    print<This,Out>(*this);
  }

  inline bool enter() {return N::template cmd<Cmds::Enter,This>(*this);}
  inline bool esc() {return N::template cmd<Cmds::Esc,This>(*this);}
  inline bool up() {return N::template cmd<Cmds::Up,This>(*this);}
  inline bool down() {return N::template cmd<Cmds::Down,This>(*this);}
  inline bool left() {return N::template cmd<Cmds::Left,This>(*this);}
  inline bool right() {return N::template cmd<Cmds::Right,This>(*this);}

  template<typename In>
  inline bool doInput() {
    trace(MDO<<"NavRoot::doInput()"<<endl);
    return In::cmd(*this);
  }
};
