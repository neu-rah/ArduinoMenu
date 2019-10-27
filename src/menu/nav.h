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
  operator Idx() const {return pos();}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"NavPos::_cmd "<<c<<endl);
    switch(c) {
      case Cmds::Up:
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

  inline Idx pos() const {return path[level];}
  inline void setPos(Idx n) {path[level]=n;}
  inline bool selected(Idx idx) const {return path[level]==idx;}
  operator Idx() const {return pos();}
  template<Cmds c,typename It,typename Nav>
  inline bool _cmd(It& it,Nav& nav) {
    trace(MDO<<"NavPos::_cmd "<<c<<endl);
    switch(c) {
      case Cmds::Up:
        if (path[level]<(it.size()-1)) path[level]++;
        else return false;
        return true;
      case Cmds::Down:
        if (path[level]>0) path[level]--;
        else return false;
        return true;
    }
    return N::template _cmd<c,It,Nav>(it,nav);
  }

  inline void debug_path() {
    _trace(
      MDO<<"  nav path:[";
      for(int n=0;n<=level;n++) MDO<<(n?",":"")<<path[n];
      MDO<<"]"<<endl;
      MDO<<"  target size:"<<size((Ref)*this)<<endl;
    );
  }

  // inline bool parentDraw() {return parentDraw(operator Ref());}
  // inline bool parentDraw(Ref ref) {return data.parentDraw(ref,ref.head());}
  inline Idx size() {return size(operator Ref());}
  inline Idx size(Ref ref) {return data.size(ref);}
  inline bool enabled(Idx n) {return data.enabled(*this,n);}
  inline bool enabled() {return data.enabled(pos());}

  template<typename Nav,typename Out>
  inline void print(Nav& nav) {
    Ref ref=*this;
    data.template printMenu<Data,Nav,Out>(data,nav,ref,ref.head());
  }

  template<Cmds c,typename Nav>
  inline bool cmd(Nav& nav) {
    if (c==Cmds::Esc) {
      close();
      return true;
    }
    Ref ref=*this;
    trace(MDO<<"Data->cmd:"<<c<<" to level:"<<level<<" idx:"<<path[level]<<endl);
    // debug_path();
    bool res=data.template cmd<c,Data,Nav>(data,nav,ref,ref.head());
    if (c==Cmds::Enter&&!res) {
      trace(MDO<<"StaticNavTree calling close by "<<c<<" returning false"<<endl);
      close();
    }
    return true;
  }

  inline void open() {
    trace(MDO<<"StaticNavTree::open"<<endl);
    trace({
      Ref ref=*this;
      assert(data.canNav(*this));
      MDO<<"yeah it handles cmds"<<endl;
      assert(level<max_depth-1);
      MDO<<"and we have depth level to focus"<<endl;
    })
    if (level>=max_depth-1) return;
    level++;
    path[level]=0;
    // debug_path();
  }

  inline void close() {
    trace(MDO<<"StaticNavTree::close"<<endl);
    if (level>0) level--;
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
    return In::cmd(*this);
  }
};
