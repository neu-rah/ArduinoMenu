#include "staticItem.h"

namespace Menu {
  template<typename F,typename S>
  template<typename Nav,typename Out,Op op>
  inline void Pair<F,S>::printMenu(Nav& nav,Out& out,PathRef ref,Idx n) {
    trace(MDO<<"Pair::printMenu... "<<ref<<":"<<n<<endl);
    if (n) tail.template printMenu<Nav,Out,op>(nav,out,ref,n-1);
    else if (ref) F::template printMenu<Nav,Out,op>(nav,out,ref.tail(),ref.head());
    else out.template printMenu<typename F::Type,Nav,op>(F::obj(),nav);
  }

  template<typename F,typename S>
  template<typename Nav,typename Out,Op op,Roles role>
  inline void Pair<F,S>::printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref,bool fullPrint) {
    trace(MDO<<"Pair::printItems top:"<<top<<" out.freeY:"<<out.freeY()<<" fullPrint:"<<fullPrint<<endl);
    if (ref) printItems<Nav,Out,op,role>(nav,out,idx,top,ref.tail(),fullPrint);
    else {
      if (!out.freeY()) return;
      if(top) tail.template printItems<Nav,Out,op,role>(nav,out,++idx,top-1,self,fullPrint);//skip scroll-out part
      else switch(op) {
        case Op::Measure:
          out.template printItem<typename F::Type,Nav,op,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
          break;
        case Op::Printing:
          trace(MDO<<"Pair::printItems changed:"<<F::changed()<<" partialDraw:"<<out.partialDraw()<<endl);
          if (fullPrint||F::changed()||!out.partialDraw())
            out.template printItem<typename F::Type,Nav,op,true>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
          else
            out.template printItem<typename F::Type,Nav,Op::Measure,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
          break;
        case Op::ClearChanges:
          F::changed(false);
      }
      tail.template printItems<Nav,Out,op,role>(nav,out,idx+1,top,self,fullPrint);
    }
  }
};
