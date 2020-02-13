#include "dynItem.h"

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
inline void Prompt<I...>::printItems(INav& nav,IOut& out,Idx idx,Idx top,PathRef ref,Op op,Roles role,bool toPrint) {
  trace(MDO<<"Prompt::printItems"<<endl);
  switch(op) {
    case Op::Measure:
    switch(role) {
      case Roles::Raw:Base::template printItems<INav,IOut,Op::Measure,Roles::Raw>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Panel: Base::template printItems<INav,IOut,Op::Measure,Roles::Panel>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Menu: Base::template printItems<INav,IOut,Op::Measure,Roles::Menu>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Title: Base::template printItems<INav,IOut,Op::Measure,Roles::Title>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Body: Base::template printItems<INav,IOut,Op::Measure,Roles::Body>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Item: Base::template printItems<INav,IOut,Op::Measure,Roles::Item>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Index: Base::template printItems<INav,IOut,Op::Measure,Roles::Index>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Cursor: Base::template printItems<INav,IOut,Op::Measure,Roles::Cursor>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Name: Base::template printItems<INav,IOut,Op::Measure,Roles::Name>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Mode: Base::template printItems<INav,IOut,Op::Measure,Roles::Mode>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Value: Base::template printItems<INav,IOut,Op::Measure,Roles::Value>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Unit: Base::template printItems<INav,IOut,Op::Measure,Roles::Unit>(nav,out,idx,top,ref,toPrint);break;
    };break;
    case Op::Printing:
    switch(role) {
      case Roles::Raw: Base::template printItems<INav,IOut,Op::Printing,Roles::Raw>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Panel: Base::template printItems<INav,IOut,Op::Printing,Roles::Panel>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Menu: Base::template printItems<INav,IOut,Op::Printing,Roles::Menu>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Title: Base::template printItems<INav,IOut,Op::Printing,Roles::Title>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Body: Base::template printItems<INav,IOut,Op::Printing,Roles::Body>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Item: Base::template printItems<INav,IOut,Op::Printing,Roles::Item>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Index: Base::template printItems<INav,IOut,Op::Printing,Roles::Index>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Cursor: Base::template printItems<INav,IOut,Op::Printing,Roles::Cursor>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Name: Base::template printItems<INav,IOut,Op::Printing,Roles::Name>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Mode: Base::template printItems<INav,IOut,Op::Printing,Roles::Mode>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Value: Base::template printItems<INav,IOut,Op::Printing,Roles::Value>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Unit: Base::template printItems<INav,IOut,Op::Printing,Roles::Unit>(nav,out,idx,top,ref,toPrint);break;
    };break;
    case Op::ClearChanges:
    switch(role) {
      case Roles::Raw: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Raw>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Panel: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Panel>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Menu: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Menu>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Title: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Title>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Body: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Body>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Item: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Item>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Index: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Index>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Cursor: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Cursor>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Name: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Name>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Mode: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Mode>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Value: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Value>(nav,out,idx,top,ref,toPrint);break;
      case Roles::Unit: Base::template printItems<INav,IOut,Op::ClearChanges,Roles::Unit>(nav,out,idx,top,ref,toPrint);break;
    };break;
  }
}
template<Expr... I>
inline void Prompt<I...>::print(INav& nav,IOut& out,Op op,Roles role,PathRef ref) {
  trace(MDO<<"Prompt::print"<<endl);
  switch(op) {
    case Op::Measure:
      switch(role) {
        case Roles::Raw: Base::template print<INav,IOut,Op::Measure,Roles::Raw>(nav,out,ref);break;
        case Roles::Panel: Base::template print<INav,IOut,Op::Measure,Roles::Panel>(nav,out,ref);break;
        case Roles::Menu: Base::template print<INav,IOut,Op::Measure,Roles::Menu>(nav,out,ref);break;
        case Roles::Title: Base::template print<INav,IOut,Op::Measure,Roles::Title>(nav,out,ref);break;
        case Roles::Body: Base::template print<INav,IOut,Op::Measure,Roles::Body>(nav,out,ref);break;
        case Roles::Item: Base::template print<INav,IOut,Op::Measure,Roles::Item>(nav,out,ref);break;
        case Roles::Index: Base::template print<INav,IOut,Op::Measure,Roles::Index>(nav,out,ref);break;
        case Roles::Cursor: Base::template print<INav,IOut,Op::Measure,Roles::Cursor>(nav,out,ref);break;
        case Roles::Name: Base::template print<INav,IOut,Op::Measure,Roles::Name>(nav,out,ref);break;
        case Roles::Mode: Base::template print<INav,IOut,Op::Measure,Roles::Mode>(nav,out,ref);break;
        case Roles::Value: Base::template print<INav,IOut,Op::Measure,Roles::Value>(nav,out,ref);break;
        case Roles::Unit: Base::template print<INav,IOut,Op::Measure,Roles::Unit>(nav,out,ref);break;
      };break;
    case Op::Printing:
      switch(role) {
        case Roles::Raw: Base::template print<INav,IOut,Op::Printing,Roles::Raw>(nav,out,ref);break;
        case Roles::Panel: Base::template print<INav,IOut,Op::Printing,Roles::Panel>(nav,out,ref);break;
        case Roles::Menu: Base::template print<INav,IOut,Op::Printing,Roles::Menu>(nav,out,ref);break;
        case Roles::Title: Base::template print<INav,IOut,Op::Printing,Roles::Title>(nav,out,ref);break;
        case Roles::Body: Base::template print<INav,IOut,Op::Printing,Roles::Body>(nav,out,ref);break;
        case Roles::Item: Base::template print<INav,IOut,Op::Printing,Roles::Item>(nav,out,ref);break;
        case Roles::Index: Base::template print<INav,IOut,Op::Printing,Roles::Index>(nav,out,ref);break;
        case Roles::Cursor: Base::template print<INav,IOut,Op::Printing,Roles::Cursor>(nav,out,ref);break;
        case Roles::Name: Base::template print<INav,IOut,Op::Printing,Roles::Name>(nav,out,ref);break;
        case Roles::Mode: Base::template print<INav,IOut,Op::Printing,Roles::Mode>(nav,out,ref);break;
        case Roles::Value: Base::template print<INav,IOut,Op::Printing,Roles::Value>(nav,out,ref);break;
        case Roles::Unit: Base::template print<INav,IOut,Op::Printing,Roles::Unit>(nav,out,ref);break;
      };break;
    case Op::ClearChanges:
      switch(role) {
        case Roles::Raw: Base::template print<INav,IOut,Op::ClearChanges,Roles::Raw>(nav,out,ref);break;
        case Roles::Panel: Base::template print<INav,IOut,Op::ClearChanges,Roles::Panel>(nav,out,ref);break;
        case Roles::Menu: Base::template print<INav,IOut,Op::ClearChanges,Roles::Menu>(nav,out,ref);break;
        case Roles::Title: Base::template print<INav,IOut,Op::ClearChanges,Roles::Title>(nav,out,ref);break;
        case Roles::Body: Base::template print<INav,IOut,Op::ClearChanges,Roles::Body>(nav,out,ref);break;
        case Roles::Item: Base::template print<INav,IOut,Op::ClearChanges,Roles::Item>(nav,out,ref);break;
        case Roles::Index: Base::template print<INav,IOut,Op::ClearChanges,Roles::Index>(nav,out,ref);break;
        case Roles::Cursor: Base::template print<INav,IOut,Op::ClearChanges,Roles::Cursor>(nav,out,ref);break;
        case Roles::Name: Base::template print<INav,IOut,Op::ClearChanges,Roles::Name>(nav,out,ref);break;
        case Roles::Mode: Base::template print<INav,IOut,Op::ClearChanges,Roles::Mode>(nav,out,ref);break;
        case Roles::Value: Base::template print<INav,IOut,Op::ClearChanges,Roles::Value>(nav,out,ref);break;
        case Roles::Unit: Base::template print<INav,IOut,Op::ClearChanges,Roles::Unit>(nav,out,ref);break;
      };break;
  }
}
