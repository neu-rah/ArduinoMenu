#include "staticIO.h"

namespace Menu {
  template<Expr...  O>
  void MenuOut<O...>::printMenu(IItem& it,INav& nav,bool fullPrint, Op op) {
    switch(op) {
      case Op::Printing: Base::template printMenu<IItem,INav,Op::Printing>(it,nav,fullPrint);break;
      case Op::Measure: Base::template printMenu<IItem,INav,Op::Measure>(it,nav,fullPrint);break;
      case Op::ClearChanges: Base::template printMenu<IItem,INav,Op::ClearChanges>(it,nav,fullPrint);break;
    };
  }
  template<Expr...  O>
  void MenuOut<O...>::printTitle(IItem& it,INav& nav, Op op) {
    switch(op) {
      case Op::Printing: Base::template printTitle<IItem,INav,Op::Printing>(it,nav);break;
      case Op::Measure: Base::template printTitle<IItem,INav,Op::Measure>(it,nav);break;
      case Op::ClearChanges: Base::template printTitle<IItem,INav,Op::ClearChanges>(it,nav);break;
    };
  }
  template<Expr...  O>
  void MenuOut<O...>::printItem(IItem& it,INav& nav,Idx n,bool s,bool e,Mode m,Op op) {
    switch(op) {
      case Op::Printing: Base::template printItem<IItem,INav,Op::Printing>(it,nav,n,s,e,m);break;
      case Op::Measure: Base::template printItem<IItem,INav,Op::Measure>(it,nav,n,s,e,m);break;
      case Op::ClearChanges: Base::template printItem<IItem,INav,Op::ClearChanges>(it,nav,n,s,e,m);
        break;
    }
  }
  template<Expr...  O>
  void MenuOut<O...>::fmt(Tag r,bool io,Idx n,bool s,bool e,Mode m,bool toPrint) {
    if(io) {
      if(toPrint) This::fmt<true,true>(r,n,s,e,m);
      else fmt<true,false>(r,n,s,e,m);
    } else {
      if(toPrint) fmt<false,true>(r,n,s,e,m);
      else fmt<false,false>(r,n,s,e,m);
    }
  }
}
