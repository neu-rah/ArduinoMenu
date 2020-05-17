#include "item.h"

namespace Menu {
  template<Expr... I>
  inline bool Prompt<I...>::cmd(Cmd c,INav& nav,Idx level,Idx aux) {
    switch(c) {
      case Cmd::None: return Base::template cmd<Cmd::None,INav>(nav,level,aux);break;
      case Cmd::Enter: return Base::template cmd<Cmd::Enter,INav>(nav,level,aux);break;
      case Cmd::Esc: return Base::template cmd<Cmd::Esc,INav>(nav,level,aux);break;
      case Cmd::Up: return Base::template cmd<Cmd::Up,INav>(nav,level,aux);break;
      case Cmd::Down: return Base::template cmd<Cmd::Down,INav>(nav,level,aux);break;
      case Cmd::Left: return Base::template cmd<Cmd::Left,INav>(nav,level,aux);break;
      case Cmd::Right: return Base::template cmd<Cmd::Right,INav>(nav,level,aux);break;
    }
    assert(false);
  }

  template<Expr... I>
  inline bool Prompt<I...>::cmdItem(Cmd c,INav& nav,Idx level,Idx aux,Idx i) {
    switch(c) {
      case Cmd::None: return Base::template cmd<Cmd::None,INav>(nav,level,aux,i);break;
      case Cmd::Enter: return Base::template cmd<Cmd::Enter,INav>(nav,level,aux,i);break;
      case Cmd::Esc: return Base::template cmd<Cmd::Esc,INav>(nav,level,aux,i);break;
      case Cmd::Up: return Base::template cmd<Cmd::Up,INav>(nav,level,aux,i);break;
      case Cmd::Down: return Base::template cmd<Cmd::Down,INav>(nav,level,aux,i);break;
      case Cmd::Left: return Base::template cmd<Cmd::Left,INav>(nav,level,aux,i);break;
      case Cmd::Right: return Base::template cmd<Cmd::Right,INav>(nav,level,aux,i);break;
    }
    assert(false);
  }

  template<Expr... I>
  inline void Prompt<I...>::printMenu(INav& nav,IOut& out,Idx level,Op op) {
    switch(op) {
      case Op::Measure: Base::template printMenu<INav,IOut,Op::Measure>(nav,out,level);break;
      case Op::Printing: Base::template printMenu<INav,IOut,Op::Printing>(nav,out,level);break;
      case Op::ClearChanges: Base::template printMenu<INav,IOut,Op::ClearChanges>(nav,out,level);break;
    }
  }

  template<Expr... I>
  inline void Prompt<I...>::printMenuItem(INav& nav,IOut& out,Idx level,Idx n,Op op) {
    switch(op) {
      case Op::Measure: Base::template printMenuItem<INav,IOut,Op::Measure>(nav,out,level,n);break;
      case Op::Printing: Base::template printMenuItem<INav,IOut,Op::Printing>(nav,out,level,n);break;
      case Op::ClearChanges: Base::template printMenuItem<INav,IOut,Op::ClearChanges>(nav,out,level,n);break;
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
  inline void Prompt<I...>::print(INav& nav,IOut& out,Idx level,Op op) {
    switch(op) {
      case Op::Measure: Base::template print<INav,IOut,Op::Measure>(nav,out,level);break;
      case Op::Printing: Base::template print<INav,IOut,Op::Printing>(nav,out,level);break;
      case Op::ClearChanges: Base::template print<INav,IOut,Op::ClearChanges>(nav,out,level);break;
    }
  }
  template<Expr... I>
  inline void Prompt<I...>::printTitle(INav& nav,IOut& out,Idx level,bool fullPrint,Op op) {
    switch(op) {
      case Op::Measure: Base::template printTitle<INav,IOut,Op::Measure>(nav,out,level,fullPrint);break;
      case Op::Printing: Base::template printTitle<INav,IOut,Op::Printing>(nav,out,level,fullPrint);break;
      case Op::ClearChanges: Base::template printTitle<INav,IOut,Op::ClearChanges>(nav,out,level,fullPrint);break;
    }
  }
};
