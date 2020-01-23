/* -*- C++ -*- */
#pragma once

#include <menu.h>
#include <vector>
template<typename M>
struct ItemArray {
  template<typename I>
  struct As:I,vector<M*> {
    using vector<M*>::vector;
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0) {
      for(auto a:*this) {
        a->template printItem<Nav,Out,op>(nav,out);
        MDO<<endl;
      }
    }
    template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
    inline void print(Nav& nav,Out& out) {
      if (role&(Roles::Title|Roles::Raw)) I::print(out);
      else out.printMenu(I::obj(),nav);
    }
  };
  template<typename I> using Open=As<I>;
};
