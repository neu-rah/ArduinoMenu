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

// template<typename M>
// struct XItemArray {
//   template<typename I>
//   struct As:vector<M*>,I {
//     using vector<M*>::vector;
//     template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     inline void printItems(Nav& nav,Out& out,Idx idx=0,Idx top=0,PathRef ref=self) {
//       _trace(MDO<<"ItemArray::printItems"<<endl);
//       if(ref) vector<M*>::operator[](ref.head())->printItems(nav,out,idx,top,ref.tail());
//       else for(auto a:*this)
//         out.template printItem
//           <decltype(*a),Nav,op,true>
//           (*a,nav,idx++,nav.selected(idx),a->enabled(),nav.mode());
//     }
//     // template<typename Nav,typename Out,Op op=Op::Printing,Roles role=Roles::Raw>
//     // inline void print(Nav& nav,Out& out,PathRef ref=self) {
//     //   _trace(MDO<<"ItemArray::print "<<role<<endl);
//     //   I::print(out);
//     //   // if (role&(Roles::Title|Roles::Raw|Roles::Item)) I::print(out);
//     //   // else out.printMenu(I::obj(),nav);
//     // }
//     inline static bool parentPrint(PathRef ref=self) {
//       return ref?
//         vector<M*>::operator[](ref.head())->parentPrint(ref.tail()):
//         I::parentPrint();
//     }
//   };
//   template<typename I> using Open=As<I>;
// };
