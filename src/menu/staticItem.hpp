#include "staticItem.h"

namespace Menu {
  // template<typename F,typename S>
  // template<typename Nav,typename Out,Op op,Idx i>
  // inline void Pair<F,S>::printMenu(Nav& nav,Out& out) {
  //   if (F::id(i)) out.template printMenu
  //     <typename F::Type,Nav,op>
  //     (F::obj(),nav,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
  //   else tail.template printMenu<Nav,Out,op,i>(nav,out);
  // }

  // template<typename F,typename S>
  // template<typename Nav,typename Out,Op op>
  // inline void Pair<F,S>::printMenu(Nav& nav,Out& out,PathRef ref,Idx n) {
  //   trace(MDO<<"Pair::printMenu... "<<ref<<":"<<n<<endl);
  //   if (n) tail.template printMenu<Nav,Out,op>(nav,out,ref,n-1);
  //   else if (ref) F::template printMenu<Nav,Out,op>(nav,out,ref.tail(),ref.head());
  //   else out.template printMenu
  //     <typename F::Type,Nav,op>
  //     (F::obj(),nav,op==Op::Printing&&(out.fullDraw()||F::changed()||out.isSame(&F::obj())));
  // }

  // template<typename F,typename S>
  // template<typename Nav,typename Out,Op op>
  // inline void Pair<F,S>::printItems(Nav& nav,Out& out,bool fullPrint,Idx idx,Idx top,PathRef ref) {
  //   trace(MDO<<"Pair::printItems top:"<<top<<" out.freeY:"<<out.freeY()<<" fullPrint:"<<fullPrint<<" changed:"<<F::changed()<<endl);
  //   if (ref) printItems<Nav,Out,op>(nav,out,fullPrint,idx,top,ref.tail());
  //   else {
  //     if (!out.freeY()) return;
  //     if(top) tail.template printItems<Nav,Out,op>(nav,out,fullPrint,++idx,top-1,self);//skip scroll-out part
  //     else if(op==Op::ClearChanges) F::changed(false);
  //     else {
  //       trace(
  //         MDO<<"Item ";
  //         F::print(nav,debugOut);
  //         MDO<<" changed:"<<F::changed()<<endl;
  //       );
  //       if (op==Op::Printing&&(fullPrint||out.fullDraw()||F::changed()))
  //         out.template printItem<typename F::Type,Nav,op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
  //       else out.template printItem<typename F::Type,Nav,op==Op::Printing?Op::Measure:op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
  //     }
  //     // else switch(op) {
  //     //   case Op::Measure:
  //     //     out.template printItem<typename F::Type,Nav,op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
  //     //     break;
  //     //   case Op::Printing:
  //     //     trace(MDO<<"Pair::printItems changed:"<<F::changed()<<" partialDraw:"<<out.partialDraw()<<endl);
  //     //     out.template printItem<typename F::Type,Nav,fullPrint,op>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
  //     //     break;
  //     //   case Op::ClearChanges:
  //     //     F::changed(false);
  //     // }
  //     tail.template printItems<Nav,Out,op>(nav,out,fullPrint,idx+1,top,self);
  //   }
  // }
};
