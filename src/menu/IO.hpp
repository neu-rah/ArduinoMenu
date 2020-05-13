#include "staticIO.h"

namespace Menu {
  template<Expr...  O>
  void MenuOut<O...>::printMenu(IItem& it,INav& nav,Idx level,bool fullPrint, Op op) {
    switch(op) {
      case Op::Printing: Base::template printMenu<IItem,INav,Op::Printing>(it,nav,level,fullPrint);break;
      case Op::Measure: Base::template printMenu<IItem,INav,Op::Measure>(it,nav,level,fullPrint);break;
      case Op::ClearChanges: Base::template printMenu<IItem,INav,Op::ClearChanges>(it,nav,level,fullPrint);break;
    };
  }
  template<Expr...  O>
  void MenuOut<O...>::printTitle(IItem& it,INav& nav,Idx level, Op op) {
    switch(op) {
      case Op::Printing: Base::template printTitle<IItem,INav,Op::Printing>(it,nav,level);break;
      case Op::Measure: Base::template printTitle<IItem,INav,Op::Measure>(it,nav,level);break;
      case Op::ClearChanges: Base::template printTitle<IItem,INav,Op::ClearChanges>(it,nav,level);break;
    };
  }
  template<Expr...  O>
  void MenuOut<O...>::printItem(IItem& it,INav& nav,Idx level,Idx n,bool s,bool e,Mode m,Op op) {
    switch(op) {
      case Op::Printing: Base::template printItem<IItem,INav,Op::Printing>(it,nav,level,n,s,e,m);break;
      case Op::Measure: Base::template printItem<IItem,INav,Op::Measure>(it,nav,level,n,s,e,m);break;
      case Op::ClearChanges: Base::template printItem<IItem,INav,Op::ClearChanges>(it,nav,level,n,s,e,m);
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
