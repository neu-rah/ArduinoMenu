/* -*- C++ -*- */
#pragma once
/**
* @file printers.h
* @author Rui Azevedo
* @brief ArduinoMenu part printers
*/

template<typename O=TextMeasure>
struct FullPrinter:public O {
  template<typename Nav,typename Out,typename I>
  inline void printMenu(Nav& nav,Out& out,I& i) {
    trace(MDO<<"FullPrinter::printMenu"<<endl);
    out.template fmtPanel<true>(nav,out,i,0);
    out.template fmtMenu<true>(nav,out,i,0);
    out.template fmtTitle<true>(nav,out,i,0);
    i.print(nav,out);
    out.template fmtTitle<false>(nav,out,i,0);
    out.template fmtBody<true>(nav,out,i,0);

    if (Out::isRange()) {
      //ensure that selection option is withing range
      //TODO: option can have variable height...
      //this is NOT taking it into account => rework
      while(out.top()>nav.pos())
        out.setTop(out.top()-1);
      while(nav.pos()>=out.top()+out.freeY())
        out.setTop(out.top()+1);
    }

    for(idx_t n=out.top();n<i.size();n++) {
      if (!out.freeY()) break;
      O::clrLine(out,O::posY());
      out.template fmtItem<true>(nav,out,*this,n);
      out.template fmtIndex<true>(nav,out,*this,n);
      out.template fmtIndex<false>(nav,out,*this,n);
      out.template fmtCursor<true>(nav,out,*this,n);
      out.template fmtCursor<false>(nav,out,*this,n);
      i.printItem(nav,out,n);
      out.template fmtItem<false>(nav,out,*this,n);
    }

    out.template fmtBody<false>(nav,out,i,0);
    out.template fmtMenu<false>(nav,out,i,0);
    out.template fmtPanel<false>(nav,out,i,0);
  }
};
