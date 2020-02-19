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
        // bool fullPrint=toPrint&&(O::fullDraw()||it.changed()||!O::isSame(&it));
        O::newView();
        // bool dp=(!O::partialDraw())||(O::partialDraw()&&!O::isSame((void*)&it));
        // if(dp) O::template fmt<Roles::Panel,true,toPrint>();
        O::template fmt<Roles::Menu,true,toPrint>();

        trace(MDO<<"FullPrinter printing title"<<" fullPrint:"<<fullPrint<<endl);
        // bool tp=toPrint&&O::partialDraw()&&!O::isSame(&it);
        //title
        // if (tp||it.changed()) {
          // if (toPrint)
          it.template printTitle<Nav,typename O::Type,op>(nav,O::obj(),fullPrint);
        // } else {
        //   O::template fmt<Roles::Item,true,false>();
        //   O::template fmt<Roles::Title,true,false>();
        //   it.template print<Nav,decltype(O::obj()),op,Roles::Title>(nav,O::obj());
        //   O::template fmt<Roles::Title,false,false>();
        //   O::template fmt<Roles::Item,false,false>();
        // }

        // This::posTop(nav);
        if (This::posTop(nav)) it.changed(true);
        // trace(MDO
        //     <<"FullPrinter printing body, fullPrint:"<<fp
        //     <<" partialDraw:"<<O::partialDraw()
        //     <<" changed:"<<it.changed()
        //     <<" isSame:"<<O::isSame(&it)
        //     <<" => fp:"<<fp<<endl
        //   );
        it.template printItems
          <Nav,typename O::Type,op>
          (nav,O::obj(),fullPrint,0,O::obj().top(),self);

        O::template fmt<Roles::Menu,false>(fullPrint);
        // if (tp) O::template fmt<Roles::Menu,false,true>();
        // else O::template fmt<Roles::Menu,false,false>();
        // if(dp) {
        //   if(tp) O::template fmt<Roles::Panel,false,true>();
        //   else O::template fmt<Roles::Panel,false,false>();
        // }
        // O::template fmt<Roles::Panel,false,fullPrint>();
        if (toPrint) O::lastDrawn(&it);
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printTitle(It& it,Nav& nav) {
        trace(MDO<<"FullPrinter::printTitle op:"<<op<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::template clrLine<toPrint>(O::orgY()+O::posY());
        O::template fmt<Roles::Item,true,toPrint>();
        O::template fmt<Roles::Title,true,toPrint>();
        it.template print<Nav,typename O::Type,op>(nav,O::obj());
        O::template fmt<Roles::Title,false,toPrint>();
        O::template fmt<Roles::Item,false,toPrint>();
      }
      template<typename It,typename Nav,Op op=Op::Printing>
      void printItem(It& it,Nav& nav,Idx n=0,bool s=false,bool e=true,Modes m=Modes::Normal) {
        trace(MDO<<"FullPrinter::printItem op:"<<op<<" posY:"<<O::posY()<<endl);
        constexpr bool toPrint=op==Op::Printing;
        O::template clrLine<toPrint>(O::orgY()+O::posY());
        O::template fmt<Roles::Item,  true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Index, true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Index, false,toPrint>(n,s,e,m);
        O::template fmt<Roles::Cursor,true ,toPrint>(n,s,e,m);
        O::template fmt<Roles::Cursor,false,toPrint>(n,s,e,m);
        // it.template printItem<O::Type,Roles::Item,toPrint>(O::obj(),n,s,e,m);
        it.template print<Nav,typename O::Type,op>(nav,O::obj());
        O::template fmt<Roles::Item,false,toPrint>(n,s,e,m);
      }
    };
  };
}
