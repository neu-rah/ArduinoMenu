/* -*- C++ -*- */
#pragma once

#include "api.h"

////////////////////////////////////////////////////////////////////////////////
// printers

//to avoid passing the output object back and forth a printer must be a top level vomposition
//because it needs do call back data for item printing
template<typename O>
struct FullPrinter:public O {
  using This=FullPrinter<O>;
  // template<typename It,typename Nav,typename Out>
  // inline void printParent(It& it,Nav& nav,Out& out) {
  //   //this is ugly but short
  //   nav.left();
  //   This::printMenu<It,Nav>(it,nav,out);
  //   nav.right();
  // }
  template<typename It,typename Nav,Op op=Op::Printing>
  void printMenu(It& it,Nav& nav) {
    _trace(MDO<<"FullPrinter::printMenu"<<endl);
    constexpr bool toPrint=op==Op::Printing;
    O::newView();
    bool dp=(!O::partialDraw())||(O::partialDraw()&&!O::isSame((void*)&it));
    if(dp) O::template fmt<Roles::Panel,true,toPrint>();
    O::template fmt<Roles::Menu,true,toPrint>();

    //title
    if (op==Op::ClearChanges) it.changed(false);

    bool tp=toPrint&&((!O::isSame(&it))||(!O::partialDraw())||it.changed());
    if (tp) {
      O::template fmt<Roles::Item,true,true>();
      O::template fmt<Roles::Title,true,true>();
      it.template print<Nav,typename O::Type,op,Roles::Title>(nav,O::obj());
      O::template fmt<Roles::Title,false,true>();
      O::template fmt<Roles::Item,false,true>();
    } else {
      it.changed(false);
      O::template fmt<Roles::Item,true,false>();
      O::template fmt<Roles::Title,true,false>();
      it.template print<Nav,decltype(O::obj()),op,Roles::Title>(nav,O::obj());
      O::template fmt<Roles::Title,false,false>();
      O::template fmt<Roles::Item,false,false>();
    }

    it.changed(This::posTop(nav));
    bool fp=toPrint&&((!O::partialDraw())||it.changed()||!O::isSame(&it));
    it.template printItems
      <Nav,typename O::Type,op,Roles::Item>
      (nav,O::obj(),0,O::obj().top());

    O::template fmt<Roles::Menu,false,toPrint>();
    if(dp) O::template fmt<Roles::Panel,false,toPrint>();
    if (toPrint) O::lastDrawn(&it);
  }
  template<typename It,typename Nav,Op op=Op::Printing,bool toPrint=true>
  void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
    if (toPrint) O::clrLine(O::orgY()+O::posY());
    O::setCursor(O::orgX(),O::orgY()+O::posY());
    O::template fmt<Roles::Item,  true ,toPrint>(n,s,e,m);
    O::template fmt<Roles::Index, true ,toPrint>(n,s,e,m);
    O::template fmt<Roles::Index, false,toPrint>(n,s,e,m);
    O::template fmt<Roles::Cursor,true ,toPrint>(n,s,e,m);
    O::template fmt<Roles::Cursor,false,toPrint>(n,s,e,m);
    // it.template printItem<O::Type,Roles::Item,toPrint>(O::obj(),n,s,e,m);
    it.template print<Nav,typename O::Type,op,Roles::Item>(nav,O::obj());
    O::template fmt<Roles::Item,false,toPrint>(n,s,e,m);
  }
};
