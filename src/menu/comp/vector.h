/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>

template<typename M>
struct ItemArray {
  template<typename I>
  struct Part:I,vector<M*> {
    using vector<M*>::vector;

    inline bool canNav(PathRef ref=self) {
      trace(MDO<<"ItemArray::canNav "<<ref<<endl);
      if (ref) return vector<M*>::operator[](ref.head())->canNav(ref.tail());
      return true;
    }

    inline size_t size(PathRef ref=self) const {
      trace(MDO<<"ItemArray::size "<<ref<<endl);
      return ref?vector<M*>::operator[](ref.head())->size(ref.tail()):vector<M*>::size();
    }

    inline bool activate(PathRef ref=self) {
      trace(MDO<<"ItemArray::activate "<<ref<<endl);
      return ref?vector<M*>::operator[](ref.head())->activate(ref.tail()):true;
    }

    template<typename Nav,typename Out,Op op=Op::Printing>
    inline void printMenu(Nav& nav,Out& out,PathRef ref=self,Idx n=0) {
      trace(MDO<<"ItemArray::printMenu"<<endl);
      if (!ref||ref.len==1&&(vector<M*>::operator[](ref.head())->parentPrint(ref.tail())))
        out.template printMenu<typename I::Type,Nav,op>(I::obj(),nav);
      else vector<M*>::operator[](ref.head())->template printMenu<Nav,Out,op>(nav,out,ref.tail());
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
