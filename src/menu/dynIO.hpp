#include "menuIO.h"

template<Expr...  O>
void MenuOut<O...>::printMenu(IItem& it,INav& nav, Op op) {
  switch(op) {
    case Op::Printing: Base::template printMenu<IItem,INav,Op::Printing>(it,nav);break;
    case Op::Measure: Base::template printMenu<IItem,INav,Op::Measure>(it,nav);break;
    case Op::ClearChanges: Base::template printMenu<IItem,INav,Op::ClearChanges>(it,nav);break;
  };
}
template<Expr...  O>
void MenuOut<O...>::printItem(IItem& it,INav& nav,Idx n,bool s,bool e,Modes m,Op op,bool toPrint) {
  switch(op) {
    case Op::Printing:
      toPrint?
      Base::template printItem<IItem,INav,Op::Printing,true>(it,nav,n,s,e,m):
      Base::template printItem<IItem,INav,Op::Printing,false>(it,nav,n,s,e,m);
    break;
    case Op::Measure:
      toPrint?
      Base::template printItem<IItem,INav,Op::Measure,true>(it,nav,n,s,e,m):
      Base::template printItem<IItem,INav,Op::Measure,false>(it,nav,n,s,e,m);
    break;
    case Op::ClearChanges:
      toPrint?
      Base::template printItem<IItem,INav,Op::ClearChanges,true>(it,nav,n,s,e,m):
      Base::template printItem<IItem,INav,Op::ClearChanges,false>(it,nav,n,s,e,m);
    break;
  }
}
