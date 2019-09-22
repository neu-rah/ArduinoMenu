/* -*- C++ -*- */
#pragma once

#include "api.h"

//navigate on a static menu!
//this is flat navigation node
//however we would like to switch target data
//because we intend to reuse this nodes and have only one
//for each nav depth level
//so this is a static flast nav. node
template<typename Data,typename N=Drift<>>
struct StaticFlatNavNode:N {
  inline static idx_t size() {return Data::size();}
  using This=StaticFlatNavNode<Data,N>;
  template<typename O,Roles P=Roles::Raw>
  inline static void printTo() {Data::template printTo<O,P,This>();}//print full item

  template<typename O,Roles P=Roles::Raw>
  inline static void printItem(size_t i) {
    Data::template printItem<O,P,This>(i);
  }

  template<typename Out,typename Nav=This>
  inline static void printMenu(Nav& nav) {
    Data::template printMenu<Out,Nav>(nav);
  }
};

template<typename N,typename Idx=int>
class NavPosBase:public N {
  public:
    using N::N;
    inline Idx pos() const {return at;}
    inline bool selected(idx_t idx) const {return at==idx;}
    // template<typename Nav>
    inline bool up() {
      if (at<N::size()-1) {at++;return true;}
      return N::up();
    }
    // template<typename Nav>
    inline bool down() {
      if (at>0) {at--;return true;}
      return N::down();
    }
    // template<typename Nav>
    inline bool enter() {
      trace(MDO<<"NavPos::enter"<<endl);
      return N::enabled(N::pos())&&N::enter();
      // return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::enter(nav));
    }
  protected:
    Idx at=0;
};

template<typename D,typename N=Drift<>>
class StaticNavNode:public N {
  public:
    using Data=D;
    StaticNavNode():target(NULL) {}
    StaticNavNode(const Data& o):target(&o) {}
    inline idx_t size() {return target->size();}
    using This=StaticFlatNavNode<Data,N>;
    template<typename O,Roles P=Roles::Raw>
    inline void printTo() {target->template printTo<O,P,This>();}//print full item

    template<typename O,Roles P=Roles::Raw>
    inline void printItem(size_t i) {
      target->template printItem<O,P,This>(i);
    }

    template<typename Out,typename Nav=This>
    inline void printMenu(Nav& nav) {
      target->template printMenu<Out,Nav>(nav);
    }
    inline void setTarget(Data* t) {target=t;}
    inline Data& getTarget(Data* t) const {return *target;}
  protected:
    const Data* target;
};

// this is the top of static navigation
template<typename N=Drift<>>
struct NavNode:N {
  using N::N;
  using This=NavNode<N>;
  // inline static idx_t size() {return Data::size();}

  template<typename O,Roles P=Roles::Raw>
  inline void printTo() {N::template printTo<O,P,This>();}//print full item

  // template<typename O,Roles P=Roles::Raw>
  // inline static void printItem(size_t i) {
  //   Data::template printItem<O,P,This>(i);
  // }

  template<typename Out>
  inline void printMenu() {
    N::template printMenu<Out,This>(*this);
  }
};

template<typename N=Drift<>>
using NavPos=NavPosBase<N>;

template<typename Path,int n,typename R=Nil>
class NavRoot:public R {
  public:
    using Data=typename Path::Data;
    NavRoot(const Data& o) {path[0]=Path(o);}
    inline bool up() {return path[level].up();}
    inline bool down() {return path[level].down();}
    inline bool enter() {return path[level].enter();}
    inline bool esc() {return path[level].esc();}
    template<typename O,Roles P=Roles::Raw>
    inline void printTo() {path[level].template printTo<O,P>();}
    template<typename Out>
    inline void printMenu() {
      path[level].template printMenu<Out>();
    }
  protected:
    idx_t level=0;
    Path path[n]{};
};
