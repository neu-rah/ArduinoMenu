#include "staticIO.h"

namespace Menu {
  template<Expr...  O>
  void MenuOut<O...>::printMenu(IItem& it,INav& nav,bool fullPrint, Op op) {
    switch(op) {
      case Op::Printing:
        if (fullPrint) Base::template printMenu<IItem,INav,true,Op::Printing>(it,nav);
        else Base::template printMenu<IItem,INav,false,Op::Printing>(it,nav);
        break;
      case Op::Measure:
        if (fullPrint) Base::template printMenu<IItem,INav,true,Op::Measure>(it,nav);
        else Base::template printMenu<IItem,INav,false,Op::Measure>(it,nav);
        break;
      case Op::ClearChanges:
        if (fullPrint) Base::template printMenu<IItem,INav,true,Op::ClearChanges>(it,nav);
        else Base::template printMenu<IItem,INav,false,Op::ClearChanges>(it,nav);
        break;
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
