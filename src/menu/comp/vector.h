/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>

template<typename M>
struct ItemArray {
  template<typename I>
  struct Part:I,vector<M*> {
    using vector<M*>::vector;
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
      trace(MDO<<"ItemArray::printItems"<<endl);
      if(ref) vector<M*>::operator[](ref.head())->printItems(nav,out,idx,top,ref.tail());
      else for(auto a:*this)
        out.template printItem
          <decltype(*a),Nav,op,true>
          (*a,nav,idx++,nav.selected(idx),a->enabled(),nav.mode());
    }
  };
};
