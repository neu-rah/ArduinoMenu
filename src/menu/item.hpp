#include "item.h"

namespace Menu {
  template<Expr... I>
  inline bool Prompt<I...>::cmd(Cmd c,INav& nav,Idx i) {
    switch(c) {
      case Cmd::None: return Base::template cmd<Cmd::None,INav>(nav,i);break;
      case Cmd::Enter: return Base::template cmd<Cmd::Enter,INav>(nav,i);break;
      case Cmd::Esc: return Base::template cmd<Cmd::Esc,INav>(nav,i);break;
      case Cmd::Up: return Base::template cmd<Cmd::Up,INav>(nav,i);break;
      case Cmd::Down: return Base::template cmd<Cmd::Down,INav>(nav,i);break;
      case Cmd::Left: return Base::template cmd<Cmd::Left,INav>(nav,i);break;
      case Cmd::Right: return Base::template cmd<Cmd::Right,INav>(nav,i);break;
    }
    assert(false);
  }

  template<Expr... I>
  inline void Prompt<I...>::printMenu(INav& nav,IOut& out,Op op) {
    switch(op) {
      case Op::Measure: Base::template printMenu<INav,IOut,Op::Measure>(nav,out);break;
      case Op::Printing: Base::template printMenu<INav,IOut,Op::Printing>(nav,out);break;
      case Op::ClearChanges: Base::template printMenu<INav,IOut,Op::ClearChanges>(nav,out);break;
    }
  }

  template<Expr... I>
  inline void Prompt<I...>::printItems(INav& nav,IOut& out,bool fullPrint,Idx idx,Idx top,Op op) {
    switch(op) {
      case Op::Measure: Base::template printItems<INav,IOut,Op::Measure>(nav,out,fullPrint,idx,top);break;
      case Op::Printing: Base::template printItems<INav,IOut,Op::Printing>(nav,out,fullPrint,idx,top);break;
      case Op::ClearChanges: Base::template printItems<INav,IOut,Op::ClearChanges>(nav,out,fullPrint,idx,top);break;
    }
  }
  template<Expr... I>
  inline void Prompt<I...>::print(INav& nav,IOut& out,Op op) {
    switch(op) {
      case Op::Measure: Base::template print<INav,IOut,Op::Measure>(nav,out);break;
      case Op::Printing: Base::template print<INav,IOut,Op::Printing>(nav,out);break;
      case Op::ClearChanges: Base::template print<INav,IOut,Op::ClearChanges>(nav,out);break;
    }
  }
  template<Expr... I>
  inline void Prompt<I...>::printTitle(INav& nav,IOut& out,bool fullPrint,Op op) {
    switch(op) {
      case Op::Measure: Base::template printTitle<INav,IOut,Op::Measure>(nav,out,fullPrint);break;
      case Op::Printing: Base::template printTitle<INav,IOut,Op::Printing>(nav,out,fullPrint);break;
      case Op::ClearChanges: Base::template printTitle<INav,IOut,Op::ClearChanges>(nav,out,fullPrint);break;
    }
  }
};
