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
    inline bool _up() {
      if (at<N::size()-1) {at++;return true;}
      return N::_up();
    }
    // template<typename Nav>
    inline bool _down() {
      if (at>0) {at--;return true;}
      return N::_down();
    }
    // template<typename Nav>
    // inline bool enter() {
    //   trace(MDO<<"NavPos::enter"<<endl);
    //   return N::enabled(N::pos())&&N::template enter<Nav>(n);
    //   // return (!nav.enabled(nav.pos()))&&(nav.activate(at)||N::enter(nav));
    // }
  protected:
    Idx at=0;
};

// template<typename D,typename N=Drift<>>
// class TreeNavNode:public N {
//   public:
//     using Data=D;
//     TreeNavNode():target(NULL) {}
//     TreeNavNode(Data& o):target(&o) {}
//     inline idx_t size() {return target->size();}
//     using This=StaticFlatNavNode<Data,N>;
//     template<typename O,Roles P=Roles::Raw>
//     inline void printTo() {target->template printTo<O,P,This>();}//print full item
//
//     template<typename O,Roles P=Roles::Raw>
//     inline void printItem(size_t i) {
//       target->template printItem<O,P,This>(i);
//     }
//
//     template<typename Out,typename Nav=This>
//     inline void printMenu(Nav& nav) {
//       target->template printMenu<Out,Nav>(nav);
//     }
//     inline void setTarget(Data* t) {target=t;}
//     inline Data& getTarget(Data* t) const {return *target;}
//     template<typename Nav>
//     inline bool up(Nav& nav) {return target->template up<Nav>(nav);}
//     template<typename Nav>
//     inline bool down(Nav& nav) {return target->template down<Nav>(nav);}
//     template<typename Nav>
//     inline bool enter(Nav& nav) {return target->template enter<Nav>(nav);}
//     template<typename Nav>
//     inline bool esc(Nav& nav) {return target->template esc<Nav>(nav);}
//   protected:
//     //problem here, this can only indicate the base of data,
//     // tree node will have different type if using static menus
//     Data* target;
// };

// this is the top of static navigation
template<typename N=Drift<>>
struct NavNode:N {
  using N::N;
  using This=NavNode<N>;

  template<typename Out>
  inline void printMenu() {
    N::template printMenu<Out,This>(*this);
  }
  // inline bool up() {return N::template up<This>(*this);}
  // inline bool down() {return N::template down<This>(*this);}
  // inline bool enter() {return N::template enter<This>(*this);}
  // inline bool esc() {return N::template esc<This>(*this);}
};

template<typename N=Drift<>>
using NavPos=NavPosBase<N>;

template<typename Data,idx_t m,typename N=Nil>
struct StaticNavRoot:N {
  using This=StaticNavRoot<Data,m,N>;
  StaticNavRoot(Data& o):root(&o) {}
  inline static constexpr idx_t maxDepth() {return m;}
  inline operator Ref() {return {level,path};}
  // inline bool up() {return root->up(*this);}
  // inline bool down() {return root->down(*this);}
  // inline bool enter() {
  //   _trace(MDO<<"StaticNavRoot::enter"<<endl);
  //   return root->enter(*this);}
  // inline bool esc() {return root->esc(*this);}
  // template<typename Out,Roles P=Roles::Raw>
  // inline void printTo() {path[level].template printTo<Out,P>();}
  // template<typename Out>
  // inline void printMenu() {
  //   path[level].template printMenu<Out>();
  // }
  inline bool up() {return N::template up<This>(*this);}
  inline bool down() {return N::template down<This>(*this);}
  inline bool enter() {return N::template enter<This>(*this);}
  inline bool esc() {return N::template esc<This>(*this);}
  inline void open() {
    path[level++]=N::pos();
    _trace(
      MDO <<endl<<"Nav::open()"<<endl
          <<"maxDepth:"<<maxDepth()<<endl
          <<"level:"<<level<<endl
          <<"path[level-1]:"<<path[level-1]<<endl
    );
  }
  template<typename Out>
  inline void printMenu() {
    root->template printMenu<Out,This>(*this);
  }
  //system default actions
  static inline bool _up() {return false;}
  static inline bool _down() {return false;}
  static inline bool _enter() {return false;}
  static inline bool _esc() {return false;}
  // protected:
    Data* root;
    idx_t level=0;
    idx_t path[m];
};

template<typename Path,int n,typename R=Nil>
class NavRoot:public R {
  public:
    using Data=typename Path::Data;
    NavRoot(Data& o) {path[0]=Path(o);}
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
    Path path[n]{};//<---- problem here, can not be homogeneous if using static menus
};
