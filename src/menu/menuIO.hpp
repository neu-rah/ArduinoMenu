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
  void MenuOut<O...>::printItem(IItem& it,INav& nav,Idx n,bool s,bool e,Modes m,Op op) {
    switch(op) {
      case Op::Printing: Base::template printItem<IItem,INav,Op::Printing>(it,nav,n,s,e,m);break;
      case Op::Measure: Base::template printItem<IItem,INav,Op::Measure>(it,nav,n,s,e,m);break;
      case Op::ClearChanges: Base::template printItem<IItem,INav,Op::ClearChanges>(it,nav,n,s,e,m);
        break;
    }
  }
}
