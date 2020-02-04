/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>

template<typename M>
struct ItemArray {
  template<typename I>
  struct Part:I,vector<M*> {
    using vector<M*>::vector;

    inline bool canNav(PathRef ref=self,Idx n=0) {
      _trace(MDO<<"ItemArray::canNav "<<ref<<" "<<n<<endl);
      if(n) return vector<M*>::operator[](n).canNav(ref);
      else if (ref) return vector<M*>::operator[](n).canNav(ref.tail(),ref.head());
      else return canNav();
    }

    inline size_t size(PathRef ref=self) const {
      _trace(MDO<<"ItemArray::size "<<ref<<endl);
      return ref?vector<M*>::operator[](ref.head())->size(ref.tail()):vector<M*>::size();
    }
    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"ItemArray::printMenu"<<endl);
      if (n) vector<M*>::operator[](n)->template printMenu<Nav,Out,op>(nav,out,ref);
      else if (ref) printMenu<Nav,Out,op>(nav,out,ref.tail(),ref.head());
      else out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
    }
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self,Idx n=0) {
      trace(MDO<<"ItemArray::printItems"<<endl);
      if(ref) vector<M*>::operator[](ref.head())->printItems(nav,out,idx,top,ref.tail());
      else for(auto a:*this)
        out.template printItem
          <decltype(*a),Nav,op,true>
          (*a,nav,idx++,nav.selected(idx),a->enabled(),nav.mode());
    }
  };
};
