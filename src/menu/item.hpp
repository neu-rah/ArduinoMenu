#include "item.h"

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
inline void Pair<F,S>::printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref) {
  trace(MDO<<"Pair::printItems top:"<<top<<" out.freeY:"<<out.freeY()<<endl);
  if (ref) printItems<Nav,Out,op,role>(nav,out,idx,top,ref.tail()/*,ref.head()*/);
  else {
    if (!out.freeY()) return;
    if(top) tail.template printItems<Nav,Out,op,role>(nav,out,++idx,top-1);//skip scroll-out part
    else switch(op) {
      case Op::Measure:
        out.template printItem<typename F::Type,Nav,op,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
        break;
      case Op::Printing:
        trace(MDO<<"Pair::printItems changed:"<<F::changed()<<" partialDraw:"<<out.partialDraw()<<endl);
        if (/*fullPrint||*/F::changed()||!out.partialDraw())
          out.template printItem<typename F::Type,Nav,op,true>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
          // out.template printItem<F,Out,true>(*this,out,idx,nav.selected(idx),F::enabled(),nav.mode());
        else
          out.template printItem<typename F::Type,Nav,Op::Measure,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
        break;
      case Op::ClearChanges:
        F::changed(false);
    }
    tail.template printItems<Nav,Out,op,role>(nav,out,idx+1,top/*,fullPrint*/);
    // else out.template printItem
    //   <typename F::Type,Nav,op,true>
    //   (F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
    // tail.template printItems<Nav,Out,op,role>(nav,out,++idx,top);
  }
}
// template<typename F,typename S>
// template<typename Nav,typename Out,Op op,Roles role>
// inline void Pair<F,S>::printItems(Nav& nav,Out& out,Idx idx,Idx top,PathRef ref,Idx n) {
//   _trace(MDO<<"Pair::printItems..."<<endl);
//   if (n) tail.template printItems<Nav,Out,op,role>(nav,out,++idx,top,ref,n-1);
//   else if (ref) F::template printItems<Nav,Out,op,role>(nav,out,idx,top,ref.tail());
//   else switch(op) {
//     case Op::Measure:
//       out.template printItem<typename F::Type,Nav,op,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
//       break;
//     case Op::Printing:
//       _trace(MDO<<"Pair::printItems changed:"<<F::changed()<<" partialDraw:"<<out.partialDraw()<<endl);
//       if (/*fullPrint||*/F::changed()||!out.partialDraw())
//         out.template printItem<typename F::Type,Nav,op,true>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
//         // out.template printItem<F,Out,true>(*this,out,idx,nav.selected(idx),F::enabled(),nav.mode());
//       else
//         out.template printItem<typename F::Type,Nav,op,false>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
//       break;
//     case Op::ClearChanges:
//       F::changed(false);
//   }
//   tail.template printItems<Nav,Out,op,role>(nav,out,idx+1,top/*,fullPrint*/);
//   // else out.template printItem<typename F::Type,Nav,op,true>(F::obj(),nav,idx,nav.selected(idx),F::enabled(),nav.mode());
// }
