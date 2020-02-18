#include "item.h"

namespace Menu {
  template<Expr... I>
  inline bool Prompt<I...>::cmd(Cmd c,INav& nav,PathRef ref) {
    switch(c) {
      case Cmd::None: return Base::template cmd<Cmd::None,INav>(nav,ref);break;
      case Cmd::Enter: return Base::template cmd<Cmd::Enter,INav>(nav,ref);break;
      case Cmd::Esc: return Base::template cmd<Cmd::Esc,INav>(nav,ref);break;
      case Cmd::Up: return Base::template cmd<Cmd::Up,INav>(nav,ref);break;
      case Cmd::Down: return Base::template cmd<Cmd::Down,INav>(nav,ref);break;
      case Cmd::Left: return Base::template cmd<Cmd::Left,INav>(nav,ref);break;
      case Cmd::Right: return Base::template cmd<Cmd::Right,INav>(nav,ref);break;
    }
    assert(false);
  }

  template<Expr... I>
  inline void Prompt<I...>::printMenu(INav& nav,IOut& out,Op op,PathRef ref) {
    switch(op) {
      case Op::Measure: Base::template printMenu<INav,IOut,Op::Measure>(nav,out,ref);break;
      case Op::Printing: Base::template printMenu<INav,IOut,Op::Printing>(nav,out,ref);break;
      case Op::ClearChanges: Base::template printMenu<INav,IOut,Op::ClearChanges>(nav,out,ref);break;
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
  template<Expr... I>
  inline void Prompt<I...>::printItems(INav& nav,IOut& out,bool fullPrint,Idx idx,Idx top,PathRef ref,Op op) {
    trace(MDO<<"Prompt::printItems"<<endl);
    switch(op) {
      case Op::Measure:
        if (fullPrint) Base::template printItems<INav,IOut,true,Op::Measure>(nav,out,idx,top,ref);
        else  Base::template printItems<INav,IOut,false,Op::Measure>(nav,out,idx,top,ref);
        break;
      case Op::Printing:
        if(fullPrint) Base::template printItems<INav,IOut,true,Op::Printing>(nav,out,idx,top,ref);
        else Base::template printItems<INav,IOut,false,Op::Printing>(nav,out,idx,top,ref);
        break;
      case Op::ClearChanges:
        if (fullPrint) Base::template printItems<INav,IOut,true,Op::ClearChanges>(nav,out,idx,top,ref);
        else Base::template printItems<INav,IOut,false,Op::ClearChanges>(nav,out,idx,top,ref);
        break;
    }
  }
  template<Expr... I>
  inline void Prompt<I...>::print(INav& nav,IOut& out,Op op,PathRef ref) {
    trace(MDO<<"Prompt::print"<<endl);
    switch(op) {
      case Op::Measure: Base::template print<INav,IOut,Op::Measure>(nav,out,ref);break;
      case Op::Printing: Base::template print<INav,IOut,Op::Printing>(nav,out,ref);break;
      case Op::ClearChanges: Base::template print<INav,IOut,Op::ClearChanges>(nav,out,ref);break;
    }
  }
};
