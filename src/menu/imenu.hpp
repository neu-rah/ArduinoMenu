#include "imenu.h"

template<typename It,typename Nav,typename Out,OutOp op/*=OutOp::Printing*/>
inline void IMenuOut::printMenu(It& it,Nav& nav,Out& out) {//TODO: remove this
  // assert(false);
  printMenu<It,Nav,Out,op>(it,nav,out);
  // printMenu(it,nav,out,op);
}

template<typename I>
void Prompt<I>::print(
  IMenuOut& out,
  Roles role/*=Roles::Raw*/,
  bool toPrint/*=true*/
) {
  switch(role) {
    case Roles::Raw:
      if(toPrint) Base::template print<IMenuOut,Roles::Raw,true>(out);
      else Base::template print<IMenuOut,Roles::Raw,false>(out);
      break;
    case Roles::Panel:
      if(toPrint) Base::template print<IMenuOut,Roles::Panel,true>(out);
      else Base::template print<IMenuOut,Roles::Panel,false>(out);
      break;
    case Roles::Menu:
      if(toPrint) Base::template print<IMenuOut,Roles::Menu,true>(out);
      else Base::template print<IMenuOut,Roles::Menu,false>(out);
      break;
    case Roles::Title:
      if(toPrint) Base::template print<IMenuOut,Roles::Title,true>(out);
      else Base::template print<IMenuOut,Roles::Title,false>(out);
      break;
    case Roles::Body:
      if(toPrint) Base::template print<IMenuOut,Roles::Body,true>(out);
      else Base::template print<IMenuOut,Roles::Body,false>(out);
      break;
    case Roles::Item:
      if(toPrint) Base::template print<IMenuOut,Roles::Item,true>(out);
      else Base::template print<IMenuOut,Roles::Item,false>(out);
      break;
    case Roles::Index:
      if(toPrint) Base::template print<IMenuOut,Roles::Index,true>(out);
      else Base::template print<IMenuOut,Roles::Index,false>(out);
      break;
    case Roles::Cursor:
      if(toPrint) Base::template print<IMenuOut,Roles::Cursor,true>(out);
      else Base::template print<IMenuOut,Roles::Cursor,false>(out);
      break;
    case Roles::Name:
      if(toPrint) Base::template print<IMenuOut,Roles::Name,true>(out);
      else Base::template print<IMenuOut,Roles::Name,false>(out);
      break;
    case Roles::Mode:
      if(toPrint) Base::template print<IMenuOut,Roles::Mode,true>(out);
      else Base::template print<IMenuOut,Roles::Mode,false>(out);
      break;
    case Roles::Value:
      if(toPrint) Base::template print<IMenuOut,Roles::Value,true>(out);
      else Base::template print<IMenuOut,Roles::Value,false>(out);
      break;
    case Roles::Unit:
      if(toPrint) Base::template print<IMenuOut,Roles::Unit,true>(out);
      else Base::template print<IMenuOut,Roles::Unit,false>(out);
      break;
  }
}


template<typename I>
void Prompt<I>::printItem(
  IMenuOut& out,
  Idx n/*=0*/,
  bool s/*=false*/,
  bool e/*=true*/,
  Modes m/*=Modes::Normal*/,
  Roles role/*=Roles::Item*/,
  bool toPrint/*=true*/
) {
  switch (role) {
    case Roles::Raw:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Raw,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Raw,false>(*this,out,n,s,e,m);
      break;
    case Roles::Panel:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Panel,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Panel,false>(*this,out,n,s,e,m);
      break;
    case Roles::Menu:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Menu,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Menu,false>(*this,out,n,s,e,m);
      break;
    case Roles::Title:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Title,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Title,false>(*this,out,n,s,e,m);
      break;
    case Roles::Body:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Body,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Body,false>(*this,out,n,s,e,m);
      break;
    case Roles::Item:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Item,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Item,false>(*this,out,n,s,e,m);
      break;
    case Roles::Index:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Index,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Index,false>(*this,out,n,s,e,m);
      break;
    case Roles::Cursor:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Cursor,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Cursor,false>(*this,out,n,s,e,m);
      break;
    case Roles::Name:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Name,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Name,false>(*this,out,n,s,e,m);
      break;
    case Roles::Mode:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Mode,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Mode,false>(*this,out,n,s,e,m);
      break;
    case Roles::Value:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Value,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Value,false>(*this,out,n,s,e,m);
      break;
    case Roles::Unit:
      if (toPrint) Base::template printItem<Item<I>,IMenuOut,Roles::Unit,true>(*this,out,n,s,e,m);
      else Base::template printItem<Item<I>,IMenuOut,Roles::Unit,false>(*this,out,n,s,e,m);
      break;
  }
}
