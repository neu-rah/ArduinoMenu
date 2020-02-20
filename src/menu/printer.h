/* -*- C++ -*- */
#pragma once

#include "api.h"

namespace Menu {
  ////////////////////////////////////////////////////////////////////////////////
  // printers

  //to avoid passing the output object back and forth a printer must be a top level vomposition
  //because it needs do call back data for item printing
  struct FullPrinter {
    template<typename O>
    struct Part:O {
      using This=FullPrinter::Part<O>;
      template<typename It,typename Nav,Op op=Op::Printing>
      void printMenu(It& it,Nav& nav,bool fullPrint) {
        trace(MDO<<"FullPrinter::printMenu fullPrint:"<<fullPrint<<" Op:"<<op<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::newView();
        O::template fmt<Roles::Menu,true,toPrint>();
        it.template printTitle<Nav,decltype(*this),op>(nav,*this,fullPrint);
        if (This::posTop(nav)) it.changed(true);
        it.template printItems
          <Nav,decltype(*this),op>
          (nav,*this,fullPrint,0,O::top(),self);
        O::template fmt<Roles::Menu,false>(fullPrint);
        if (toPrint) O::lastDrawn(&it);
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printTitle(It& it,Nav& nav) {
        trace(MDO<<"FullPrinter::printTitle op:"<<op<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::template clrLine<toPrint>(O::posY());
        O::template fmt<Roles::Item,true,toPrint>();
        O::template fmt<Roles::Title,true,toPrint>();
        it.template print<Nav,This,op>(nav,*this);
        O::template fmt<Roles::Title,false,toPrint>();
        O::template fmt<Roles::Item,false,toPrint>();
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
        trace(MDO<<"FullPrinter::printItem op:"<<op<<" posY:"<<O::posY()<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::template clrLine<toPrint>(O::posY());
        O::template fmt<Roles::Item,  true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Index, true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Index, false,toPrint>(n,s,e,m);
        O::template fmt<Roles::Cursor,true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Cursor,false,toPrint>(n,s,e,m);
        // it.template printItem<O::Type,Roles::Item,toPrint>(O::obj(),n,s,e,m);
        it.template print<Nav,This,op>(nav,*this);
        O::template fmt<Roles::Item,false,toPrint>(n,s,e,m);
      }
    };
  };
}
